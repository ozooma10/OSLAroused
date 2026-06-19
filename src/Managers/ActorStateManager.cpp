#include "ActorStateManager.h"
#include "Utilities/Utils.h"
#include "Papyrus/Papyrus.h"
#include "PersistedData.h"
#include "Settings.h"
#include "Managers/ArousalManager.h"
#include "Managers/ArousalSystem/ArousalSystemOSL.h"

namespace
{
	// sentinel sos value representing "flaccid". (outside the valid SOSBend [-9, 9] range)
	constexpr int kSosFlaccidState = -100;
}

bool IsActorNaked(RE::Actor* actorRef)
{
	return Utilities::Actor::IsNaked(actorRef);
}

void ActorStateManager::UpdateSOSAnimation(RE::Actor* actorRef, float arousal)
{
	if (!actorRef || !Settings::GetSingleton()->GetEnableSOSIntegration()) {
		return;
	}

	// get the desired sos state as an int. either kSosFlaccidState or a bend bucket in [-9, 9]
	// should mirror the old UpdateSOSPosition mapping (pos = arousal/4 - 14, clamped), but with a sentinel for flaccid instead of bucket -10
	int desiredState;
	if (Utilities::Actor::IsDead(actorRef)) {
		desiredState = kSosFlaccidState;
	} else if (Utilities::Actor::IsParticipatingInScene(actorRef)) {
		// in-scene animations own sos state, so drop any cached state so first update always re-asserts arousal-based bend when the scene ends.
		std::scoped_lock lock(m_SosStateLock);
		m_SosStateCache.erase(actorRef->formID);
		return;
	} else {
		int pos = (static_cast<int>(arousal) / 4) - 14;
		if (pos < -9) {
			desiredState = kSosFlaccidState;
		} else if (pos > 9) {
			desiredState = 9;
		} else {
			desiredState = pos;
		}
	}

	// Dedup: skip if the schlong is already in this state for this actor. This is what
	// collapses a stable crowd from one animation event per actor per cycle to ~zero.
	{
		std::scoped_lock lock(m_SosStateLock);
		auto it = m_SosStateCache.find(actorRef->formID);
		if (it != m_SosStateCache.end() && it->second == desiredState) {
			return;
		}
		m_SosStateCache[actorRef->formID] = desiredState;
	}

	// NotifyAnimationGraph must run on the main thread, so marshal it (mirrors the
	// Papyrus::Events::Send* helpers and RunWorldArousalUpdate). Capture a handle so an
	// actor that unloads before the task runs is simply skipped.
	const RE::BSFixedString animEvent = (desiredState == kSosFlaccidState)
		? RE::BSFixedString("SOSFlaccid")
		: RE::BSFixedString(("SOSBend" + std::to_string(desiredState)).c_str());
	auto actorHandle = actorRef->GetHandle();
	SKSE::GetTaskInterface()->AddTask([actorHandle, animEvent]() {
		auto actorPtr = actorHandle.get();
		if (!actorPtr) {
			return;
		}
		auto* actor = actorPtr.get();
		if (!actor || !actor->Is3DLoaded()) {
			return;
		}
		actor->NotifyAnimationGraph(animEvent);
	});
}

bool ActorStateManager::GetActorArousalLocked(RE::Actor* actorRef)
{
	if (!actorRef) {
		return true;
	}
	return PersistedData::IsArousalLockedData::GetSingleton()->GetData(actorRef->formID, false);
}

void ActorStateManager::SetActorArousalLocked(RE::Actor* actorRef, bool locked)
{
	if (!actorRef) {
		return;
	}
	PersistedData::IsArousalLockedData::GetSingleton()->SetData(actorRef->formID, locked);
	Utilities::Factions::GetSingleton()->SetFactionRank(actorRef, FactionType::sla_Arousal_Locked, locked ? 0 : -2);
}

bool ActorStateManager::GetActorNaked(RE::Actor* actorRef)
{
	return m_ActorNakedStateCache(actorRef);
}

void ActorStateManager::ActorNakedStateChanged(RE::Actor* actorRef, bool newNaked)
{
	if (!actorRef) {
		SKSE::log::warn("ActorNakedStateChanged called with null actor");
		return;
	}

	SKSE::log::trace("ActorNakedStateChanged: Actor: {} Naked: {}", actorRef->GetDisplayFullName(), newNaked);
	m_ActorNakedStateCache.UpdateItem(actorRef, newNaked);
	Papyrus::Events::SendActorNakedUpdatedEvent(actorRef, newNaked);

	//Actor Naked updated so remove libido cache entry to force refresh on next fetch
			//Only emit update events for OSL mode
	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ActorLibidoModifiersUpdated(actorRef);
	}
}

void ActorStateManager::ClearAllNakedStates()
{
	m_ActorNakedStateCache.ClearAll();
}

bool ActorStateManager::GetActorSpectatingNaked(RE::Actor* actorRef)
{
	if (!actorRef) {
		return false;
	}

	std::scoped_lock lock(m_SpectatingLock);
	auto it = m_NakedSpectatingMap.find(actorRef);
	if (it != m_NakedSpectatingMap.end()) {
		// Check if spectating is still recent (within 0.1 game hours)
		float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
		if (currentTime - it->second.lastUpdateTime < 0.1f) {
			return true;
		}
	}
	return false;
}

float ActorStateManager::GetSpectatingMaxNudityScore(RE::Actor* actorRef)
{
	if (!actorRef) {
		return 0.0f;
	}

	std::scoped_lock lock(m_SpectatingLock);
	auto it = m_NakedSpectatingMap.find(actorRef);
	if (it != m_NakedSpectatingMap.end()) {
		// Check if spectating is still recent (within 0.1 game hours)
		float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
		if (currentTime - it->second.lastUpdateTime < 0.1f) {
			return it->second.maxNudityScore;
		}
	}
	return 0.0f;
}

void ActorStateManager::UpdateActorsSpectating(std::map<RE::Actor*, float> spectatorNudityScores)
{
	float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
	const float timeoutThreshold = 0.1f; // 6 minutes at default timescale

	// Collect the actors whose libido cache needs invalidating, and apply that
	// AFTER releasing m_SpectatingLock. ActorLibidoModifiersUpdated takes the libido
	// cache's mutex, and that cache's miss callback reads this map under
	// m_SpectatingLock - invalidating while holding m_SpectatingLock would invert the
	// lock order and can deadlock against the Papyrus VM thread.
	std::vector<RE::Actor*> actorsToInvalidate;

	{
		std::scoped_lock lock(m_SpectatingLock);

		// First pass: Remove spectators who are no longer actively viewing OR timed out
		for (auto itr = m_NakedSpectatingMap.begin(); itr != m_NakedSpectatingMap.end();) {
			bool isStillSpectating = spectatorNudityScores.find(itr->first) != spectatorNudityScores.end();
			bool isTimedOut = (currentTime - itr->second.lastUpdateTime) >= timeoutThreshold;

			if (!isStillSpectating || isTimedOut) {
				actorsToInvalidate.push_back(itr->first);
				itr = m_NakedSpectatingMap.erase(itr);
			} else {
				itr++;
			}
		}

		// Second pass: Update or add active spectators
		for (const auto& [spectator, maxNudityScore] : spectatorNudityScores) {
			auto it = m_NakedSpectatingMap.find(spectator);
			bool isNewSpectator = (it == m_NakedSpectatingMap.end());
			float oldScore = isNewSpectator ? 0.0f : it->second.maxNudityScore;

			// Update or create spectating data
			SpectatingData data;
			data.maxNudityScore = maxNudityScore;
			data.lastUpdateTime = currentTime;
			m_NakedSpectatingMap[spectator] = data;

			// Invalidate cache if new spectator or if nudity score changed significantly (>3 points)
			if (isNewSpectator || std::abs(maxNudityScore - oldScore) > 3.0f) {
				actorsToInvalidate.push_back(spectator);
			}
		}
	}

	// Purge libido modifier caches for OSL mode, now that m_SpectatingLock is released.
	if (!actorsToInvalidate.empty()) {
		if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
			for (auto* actor : actorsToInvalidate) {
				oslSystem->ActorLibidoModifiersUpdated(actor);
			}
		}
	}
}

bool ActorStateManager::IsHumanoidActor(RE::Actor* actorRef)
{
	if (!actorRef) {
		return false;
	}

	if (!m_CreatureKeyword) {
		m_CreatureKeyword = (RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId);
		if (!m_CreatureKeyword) {
			SKSE::log::error("Failed to load ActorTypeCreature keyword (FormID: {:X})", kActorTypeCreatureKeywordFormId);
		}
	}
	if (!m_AnimalKeyword) {
		m_AnimalKeyword = (RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId);
		if (!m_AnimalKeyword) {
			SKSE::log::error("Failed to load ActorTypeAnimal keyword (FormID: {:X})", kActorTypeAnimalKeywordFormId);
		}
	}

	if (m_CreatureKeyword && m_AnimalKeyword) {
		return !actorRef->HasKeyword(m_CreatureKeyword) && !actorRef->HasKeyword(m_AnimalKeyword);
	}

	// If keywords failed to load, log warning and assume humanoid to prevent blocking all functionality
	SKSE::log::warn("IsHumanoidActor: Keywords not loaded, defaulting to humanoid for actor {}", actorRef->GetDisplayFullName());
	return true;
}

void ActorStateManager::HandlePlayerArousalUpdated(RE::Actor* actorRef, float newArousal)
{
	//Notif logic from sla
	if (newArousal <= 20 && (m_PreviousPlayerArousal > 20 || m_LastNotificationTime + 0.5 <= RE::Calendar::GetSingleton()->GetDaysPassed()))
	{
		if (m_WasPlayerDispleased)
		{
			RE::SendHUDMessage::ShowHUDMessage("$OSL_NotificationArousal20Displeased");
			m_WasPlayerDispleased = false;
		}
		else
		{
			RE::SendHUDMessage::ShowHUDMessage("$OSL_NotificationArousal20");
		}
		m_LastNotificationTime = RE::Calendar::GetSingleton()->GetDaysPassed();
	}
	else if (newArousal >= 90 && (m_PreviousPlayerArousal < 90 || m_LastNotificationTime + 0.2 <= RE::Calendar::GetSingleton()->GetDaysPassed()))
	{
		RE::SendHUDMessage::ShowHUDMessage("$OSL_NotificationArousal90");
		m_LastNotificationTime = RE::Calendar::GetSingleton()->GetDaysPassed();
	}
	else if (newArousal >= 70 && (m_PreviousPlayerArousal < 70 || m_LastNotificationTime + 0.3 <= RE::Calendar::GetSingleton()->GetDaysPassed()))
	{
		RE::SendHUDMessage::ShowHUDMessage("$OSL_NotificationArousal70");
		m_LastNotificationTime = RE::Calendar::GetSingleton()->GetDaysPassed();
	}
	else if (newArousal >= 50 && (m_PreviousPlayerArousal < 50 || m_LastNotificationTime + 0.4 <= RE::Calendar::GetSingleton()->GetDaysPassed()))
	{
		RE::SendHUDMessage::ShowHUDMessage("$OSL_NotificationArousal50");
		m_LastNotificationTime = RE::Calendar::GetSingleton()->GetDaysPassed();
	}

	m_PreviousPlayerArousal = newArousal;
}

void ActorStateManager::OnActorArousalUpdated(RE::Actor* actorRef, float newArousal)
{
	if (!actorRef) {
		return;
	}

	if (actorRef->IsPlayer()) {
		HandlePlayerArousalUpdated(actorRef, newArousal);
	}
	else {
		RE::Actor* mostArousedActor = nullptr;
		if (m_MostArousedActor.get()) {
			mostArousedActor = m_MostArousedActor.get().get();
		}

		if (mostArousedActor && mostArousedActor != actorRef)
		{
			if (mostArousedActor->GetCurrentLocation() == actorRef->GetCurrentLocation())
			{
				if (m_MostArousedActorArousal <= newArousal)
				{
					m_MostArousedActor = actorRef;
					m_MostArousedActorArousal = newArousal;
				}
			}
			else
			{
				m_MostArousedActor = actorRef;
				m_MostArousedActorArousal = newArousal;
			}
		}
		else
		{
			m_MostArousedActor = actorRef;
			m_MostArousedActorArousal = newArousal;
		}
	}
}

RE::Actor* ActorStateManager::GetMostArousedActorInLocation()
{
	if (m_MostArousedActor.get()) {
		return m_MostArousedActor.get().get();
	}
	return nullptr;
}


