#include "ActorStateManager.h"
#include "Utilities/Utils.h"
#include "Papyrus/Papyrus.h"
#include "Managers/ArousalManager.h"

bool IsActorNaked(RE::Actor* actorRef)
{
	return Utilities::Actor::IsNaked(actorRef);
}

bool ActorStateManager::GetActorNaked(RE::Actor* actorRef)
{
	return m_ActorNakedStateCache(actorRef);
}

void ActorStateManager::ActorNakedStateChanged(RE::Actor* actorRef, bool newNaked)
{
	m_ActorNakedStateCache.UpdateItem(actorRef, newNaked);
	Papyrus::Events::SendActorNakedUpdatedEvent(actorRef, newNaked);
	
	//Actor Naked updated so remove libido cache entry to force refresh on next fetch
			//Only emit update events for OSL mode
	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ActorLibidoModifiersUpdated(actorRef);
	}
}

bool ActorStateManager::GetActorSpectatingNaked(RE::Actor* actorRef)
{
	if (const auto lastViewedGameTime = m_NakedSpectatingMap[actorRef]) {
		if (RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastViewedGameTime < 0.1f) {
			return true;
		}
	}
	return false;
}

void ActorStateManager::UpdateActorsSpectating(std::set<RE::Actor*> spectators)
{
	//Remove any old spectators from map who are not in spectators set
	//Need to do this to purge libido modifier cache
	for (auto itr = m_NakedSpectatingMap.begin(); itr != m_NakedSpectatingMap.end();) {
		if (!spectators.contains((*itr).first)) {
			if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
				oslSystem->ActorLibidoModifiersUpdated((*itr).first);
			}
			itr = m_NakedSpectatingMap.erase(itr);
		} else {
			itr++;
		}
	}

	float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
	for (const auto spectator : spectators) {
		m_NakedSpectatingMap[spectator] = currentTime;
		if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
			oslSystem->ActorLibidoModifiersUpdated(spectator);
		}
	}
}

bool ActorStateManager::IsHumanoidActor(RE::Actor* actorRef)
{
	if (!m_CreatureKeyword) {
		m_CreatureKeyword = (RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId);
	}
	if (!m_AnimalKeyword) {
		m_AnimalKeyword = (RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId);
	}

	if (m_CreatureKeyword && m_AnimalKeyword) {
		return !actorRef->HasKeyword(m_CreatureKeyword) && !actorRef->HasKeyword(m_AnimalKeyword);
	}

	return false;
}

void ActorStateManager::HandlePlayerArousalUpdated(RE::Actor* actorRef, float newArousal)
{
	//Notif logic from sla
	if (newArousal <= 20 && (m_PreviousPlayerArousal > 20 || m_LastNotificationTime + 0.5 <= RE::Calendar::GetSingleton()->GetDaysPassed()))
	{
		if (m_WasPlayerDispleased)
		{
			RE::DebugNotification("$OSL_NotificationArousal20Displeased");
			m_WasPlayerDispleased = false;
		}
		else
		{
			RE::DebugNotification("$OSL_NotificationArousal20");
		}
		m_LastNotificationTime = RE::Calendar::GetSingleton()->GetDaysPassed();
	}
	else if (newArousal >= 90 && (m_PreviousPlayerArousal < 90 || m_LastNotificationTime + 0.2 <= RE::Calendar::GetSingleton()->GetDaysPassed()))
	{
		RE::DebugNotification("$OSL_NotificationArousal90");
		m_LastNotificationTime = RE::Calendar::GetSingleton()->GetDaysPassed();
	}
	else if (newArousal >= 70 && (m_PreviousPlayerArousal < 70 || m_LastNotificationTime + 0.3 <= RE::Calendar::GetSingleton()->GetDaysPassed()))
	{
		RE::DebugNotification("$OSL_NotificationArousal70");
		m_LastNotificationTime = RE::Calendar::GetSingleton()->GetDaysPassed();
	}
	else if (newArousal >= 50 && (m_PreviousPlayerArousal < 50 || m_LastNotificationTime + 0.4 <= RE::Calendar::GetSingleton()->GetDaysPassed()))
	{
		RE::DebugNotification("$OSL_NotificationArousal50");
		m_LastNotificationTime = RE::Calendar::GetSingleton()->GetDaysPassed();
	}

	m_PreviousPlayerArousal = newArousal;
}

void ActorStateManager::OnActorArousalUpdated(RE::Actor* actorRef, float newArousal)
{
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


