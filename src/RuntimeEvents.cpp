#include "RuntimeEvents.h"
#include "PersistedData.h"
#include "Settings.h"
#include "Managers/ArousalManager.h"
#include "Managers/ArousalSystem/ArousalSystemOSL.h"
#include "Managers/SceneManager.h"
#include "Managers/ActorStateManager.h"
#include "Papyrus/Papyrus.h"

#include "Integrations/DevicesIntegration.h"
#include "Integrations/ANDIntegration.h"
#include "Integrations/ANDFactionIndices.h"

#include "Utilities/Utils.h"

RE::BSEventNotifyControl RuntimeEvents::OnEquipEvent::ProcessEvent(const RE::TESEquipEvent* equipEvent, RE::BSTEventSource<RE::TESEquipEvent>*)
{
	if (!equipEvent || !equipEvent->actor || !equipEvent->baseObject) {
		//Something wrong with this event dont handle
		return RE::BSEventNotifyControl::kContinue;
	}

	auto equipActor = equipEvent->actor.get();

	auto equipmentForm = RE::TESForm::LookupByID(equipEvent->baseObject);
	if (!equipmentForm || !equipmentForm->IsArmor()) {
		return RE::BSEventNotifyControl::kContinue;
	}

	auto player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return RE::BSEventNotifyControl::kContinue;
	}

	//only send naked update events if actor is closeish to player
	const float guardDist = 5024; //Only process gear change for actors within 5024 units
	if (equipActor->IsPlayer() || player->GetPosition().GetSquaredDistance(equipActor->GetPosition()) < (guardDist * guardDist)) {
		const auto armor = equipmentForm->As<RE::TESObjectARMO>();

		if (armor) {
			if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kBody)) {
				//This is body armor so send Change of naked state based on if equipped or not
				ActorStateManager::GetSingleton()->ActorNakedStateChanged(static_cast<RE::Actor*>(equipEvent->actor.get()), !equipEvent->equipped);
			} else if (const auto keywordForm = armor->As<RE::BGSKeywordForm>(); keywordForm && keywordForm->keywords) {
				//Check for ArmorCuirass keyword as a backup
				for (uint32_t i = 0; i < keywordForm->numKeywords; i++) {
					if (keywordForm->keywords[i] && keywordForm->keywords[i]->formEditorID == "ArmorCuirass") {
						ActorStateManager::GetSingleton()->ActorNakedStateChanged(static_cast<RE::Actor*>(equipEvent->actor.get()), !equipEvent->equipped);
						break;
					}
				}
			}

			//A "counts as clothing" piece toggles nudity suppression for the wearer.
			//It may not be body/cuirass armor, so refresh the baseline cache directly.
			if (PersistedData::CountsAsClothingData::GetSingleton()->GetData(armor->formID, false)) {
				if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
					oslSystem->ActorLibidoModifiersUpdated(static_cast<RE::Actor*>(equipEvent->actor.get()));
				}
			}
		}

		//Changed equipped armor so update devices
		DevicesIntegration::GetSingleton()->ActiveEquipmentChanged(static_cast<RE::Actor*>(equipEvent->actor.get()), equipmentForm, equipEvent->equipped);
	}
	return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl RuntimeEvents::OnSleepStopEvent::ProcessEvent(const RE::TESSleepStopEvent* sleepStopEvent, RE::BSTEventSource<RE::TESSleepStopEvent>*)
{
	if (!sleepStopEvent) {
		return RE::BSEventNotifyControl::kContinue;
	}

	//Interrupted sleep (e.g. attacked while sleeping) does not count as restful arousal gain
	if (sleepStopEvent->interrupted) {
		return RE::BSEventNotifyControl::kContinue;
	}

	const float sleepGain = Settings::GetSingleton()->GetSleepArousalGain();
	if (sleepGain <= 0.f) {
		return RE::BSEventNotifyControl::kContinue;
	}

	//Sleep is player-driven, so the effect applies to the player only
	auto player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return RE::BSEventNotifyControl::kContinue;
	}

	SKSE::log::trace("OnSleepStopEvent: applying sleep arousal gain {} to player", sleepGain);
	Arousal::ModifyArousal(player, sleepGain);

	return RE::BSEventNotifyControl::kContinue;
}


std::vector<RE::ActorHandle> GetNearbyActorsInCell(RE::Actor* source);

std::vector<RE::Actor*> GetNearbySpectatingActors(RE::Actor* source, float radius);

void HandleAdultScenes(std::vector<SceneManager::SceneData> activeScenes, float )
{
	float scanDistance = Settings::GetSingleton()->GetScanDistance();
	
	std::set<RE::ActorHandle> spectatingActors;
	for (const auto scene : activeScenes) {
		if (scene.Participants.size() <= 0) {
			SKSE::log::warn("HandleAdultScenes: Skipping sceneid: {} no participants found", scene.SceneId);
			continue;
		}

		if(!scene.Participants[0].get()) {
			SKSE::log::warn("HandleAdultScenes: Skipping sceneid: {} first participant is null", scene.SceneId);
			continue;
		}
		const auto spectators = GetNearbySpectatingActors(scene.Participants[0].get().get(), scanDistance);
		for (const auto spectator : spectators) {
			spectatingActors.insert(spectator->GetHandle());
		}
	}
	SceneManager::GetSingleton()->UpdateSceneSpectators(spectatingActors);
}

namespace
{
	// Bounds outstanding world-update tasks to one, so ticks can't pile up while
	// the game isn't draining the SKSE task queue (e.g. at the main menu) or if a
	// scan ever outlasts the tick interval.
	std::atomic<bool> g_arousalUpdatePending{ false };
}

// Runs the actual world scan on the main game thread (marshalled from the
// background ticker by WorldChecks::ArousalUpdateLoop). Every engine read below
// requires a loaded world and the main thread.
static void RunWorldArousalUpdate()
{
	// Bail out if there is no loaded game world yet (main menu / load transition),
	// where these engine singletons are null. This is the crash that was being
	// hit: RE::Calendar::GetSingleton() returned null and GetHoursPassed()
	// dereferenced it on the background ticker thread.
	auto* calendar = RE::Calendar::GetSingleton();
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!calendar || !player) {
		return;
	}

	auto* ticker = WorldChecks::ArousalUpdateTicker::GetSingleton();
	const float curHours = calendar->GetHoursPassed();

	// First tick after load: establish the time baseline and skip, so we don't
	// apply a full (clamped) day of elapsed game time on the very first update.
	if (ticker->LastUpdatePollGameTime < 0.f) {
		ticker->LastUpdatePollGameTime = curHours;
		ticker->LastNearbyArousalUpdateGameTime = curHours;
		return;
	}

	// Clamp to 24 hours (1 full day) to allow proper convergence while preventing extreme jumps
	float elapsedGameTimeSinceLastCheck = std::clamp(curHours - ticker->LastUpdatePollGameTime, 0.f, 24.f);
	float elapsedGameTimeSinceLastNearbyArousalCheck = std::clamp(curHours - ticker->LastNearbyArousalUpdateGameTime, 0.f, 24.f);
	ticker->LastUpdatePollGameTime = curHours;

	if (elapsedGameTimeSinceLastCheck <= 0) {
		return;
	}

	const auto activeScenes = SceneManager::GetSingleton()->GetAllScenes();
	//SKSE::log::trace("ArousalUpdateLoop: Found {} active scenes. ElapsedTimeSinceLastCheck: {}", activeScenes.size(), elapsedGameTimeSinceLastCheck);
	if (activeScenes.size() > 0) {
		HandleAdultScenes(activeScenes, elapsedGameTimeSinceLastCheck);
	}

	// Perform nearby arousal updates if 0.1 game hours (6 minutes at default timescale) have passed
	bool performNearbyArousalUpdates = elapsedGameTimeSinceLastNearbyArousalCheck > Settings::GetSingleton()->GetArousalUpdateInterval();
	if (performNearbyArousalUpdates) {
		ticker->LastNearbyArousalUpdateGameTime = curHours;
	}


	// Map of spectator -> maximum nudity score they're viewing
	std::map<RE::Actor*, float> spectatorNudityScores;
	float scanDistance = Settings::GetSingleton()->GetScanDistance();
	const auto nearbyActors = GetNearbyActorsInCell(player);
	const auto actorStateManager = ActorStateManager::GetSingleton();
	const bool useANDIntegration = Settings::GetSingleton()->GetUseANDIntegration() &&
	                               Integrations::ANDIntegration::GetSingleton()->IsAvailable();

	for (const auto actorHandle : nearbyActors) {
		auto actorPtr = actorHandle.get();
		if (!actorPtr) {
			continue;
		}
		auto actor = actorPtr.get();
		if (!actor || !actorStateManager->IsHumanoidActor(actor)) {
			continue;
		}

		if (Utilities::Actor::IsDead(actor)) {
			continue;
		}

		// Check if actor is naked or partially nude
		bool isNakedOrPartiallyNude = false;
		float nudityScore = 0.0f;

		// If AND integration is enabled, use it for more sophisticated nudity detection
		if (useANDIntegration) {
			nudityScore = Integrations::ANDIntegration::GetSingleton()->GetANDNudityScore(actor);
			isNakedOrPartiallyNude = (nudityScore > 0.0f);
			if (isNakedOrPartiallyNude) {
				SKSE::log::trace("ArousalUpdateLoop: Actor {} has AND nudity score {}",
				             actor->GetDisplayFullName(), nudityScore);
			}
		} else {
			// Fallback to legacy binary naked check
			isNakedOrPartiallyNude = actorStateManager->GetActorNaked(actor);
			if (isNakedOrPartiallyNude) {
				// For legacy mode, treat as full nudity (use configured Nude baseline)
				nudityScore = Settings::GetSingleton()->GetANDFactionBaseline(Integrations::ANDFactionIndex::NUDE);
				SKSE::log::trace("ArousalUpdateLoop: Actor {} is naked (legacy check)",
				             actor->GetDisplayFullName());
			}
		}

		// A worn piece flagged "counts as clothing" makes the wearer read as clothed to
		// everyone: a flagged-but-nude actor is neither a spectator nudity source nor an
		// exhibitionist source. (Their own nudity baseline is suppressed separately in
		// ANDIntegration::GetNudityBaselineModifier.) Checked only for already-detected-nude
		// actors so the worn-inventory walk is skipped for the clothed majority. Calling
		// IsWearingClothingOverride here is main-thread-safe (RunWorldArousalUpdate is marshalled).
		if (isNakedOrPartiallyNude && Utilities::Actor::IsWearingClothingOverride(actor)) {
			isNakedOrPartiallyNude = false;
			nudityScore = 0.0f;
		}

		// If the actor is naked or partially nude, get nearby spectators to update spectator array
		if (actorStateManager->IsHumanoidActor(actor) && isNakedOrPartiallyNude) {
			const auto spectators = GetNearbySpectatingActors(actor, scanDistance);
			for (const auto spectator : spectators) {
				// Track the maximum nudity score this spectator is viewing
				auto it = spectatorNudityScores.find(spectator);
				if (it == spectatorNudityScores.end()) {
					spectatorNudityScores[spectator] = nudityScore;
				} else {
					spectatorNudityScores[spectator] = std::max(it->second, nudityScore);
				}

				// HandleSpectatingNaked already scales gains based on AND score internally (SLA mode only)
				ArousalManager::GetSingleton()->GetArousalSystem().HandleSpectatingNaked(spectator, actor, elapsedGameTimeSinceLastCheck);
			}
		}
	}

	ActorStateManager::GetSingleton()->UpdateActorsSpectating(spectatorNudityScores);

	if (performNearbyArousalUpdates) {
		for (const auto actorHandle : nearbyActors) {
			auto actorPtr = actorHandle.get();
			if (!actorPtr) {
				continue;
			}
			auto actor = actorPtr.get();
			if (!actor) {
				continue;
			}
			
			//Get the arousal to trigger an update
			Arousal::GetArousal(actor, true);
		}

		//If we are updating all nearby actors, then we want to update the stored nearby actors array, and emit the sla_UpdateComplete event
		Papyrus::Events::SendUpdateCompleteEvent(static_cast<float>(nearbyActors.size()));
		ticker->SetLastScannedActors(nearbyActors);
	}
}

void WorldChecks::ArousalUpdateLoop()
{
	// The ticker invokes this on a detached background thread (see Utilities::Ticker).
	// None of the world/engine state touched by the update (Calendar, TES, cell
	// reference lists, actor AI/detection, faction data) is safe to read off the
	// main game thread, so marshal the real work onto the main thread via the SKSE
	// task interface - the same pattern Papyrus.cpp uses for actor mutations.
	if (g_arousalUpdatePending.exchange(true)) {
		return;  // a previous update is still queued/running; skip this tick
	}
	SKSE::GetTaskInterface()->AddTask([]() {
		RunWorldArousalUpdate();
		g_arousalUpdatePending.store(false);
	});
}

std::vector<RE::ActorHandle> GetNearbyActorsInCell(RE::Actor* source)
{
	std::vector<RE::ActorHandle> nearbyActors;

	if (!source || !source->parentCell) {
		SKSE::log::warn("GetNearbyActorsInCell - source can not be null");
		return nearbyActors;
	}

	float scanDistance = Settings::GetSingleton()->GetScanDistance();

	Utilities::World::ForEachReferenceInRange(source, scanDistance, [&](RE::TESObjectREFR& ref) {
		auto refBase = ref.GetBaseObject();
		auto actor = ref.As<RE::Actor>();
		if (actor && !actor->IsDisabled() && !actor->IsChild() && (ref.Is(RE::FormType::NPC) || (refBase && refBase->Is(RE::FormType::NPC)))) {
			nearbyActors.push_back(actor->GetHandle());
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});

	return nearbyActors;
}

std::vector<RE::Actor*> GetNearbySpectatingActors(RE::Actor* source, float radius)
{
	std::vector<RE::Actor*> nearbyActors;

	if (!source || !source->parentCell) {
		SKSE::log::warn("GetNearbySpectatingActors - source can not be null");
		return nearbyActors;
	}

	//OAroused algo. Anyone nearer than force distance will have there arousal modified [0.125 is 1/8th]
	float forceDetectDistance = radius * 0.125f;
	//Square distances since we check against squared dist (GetSquaredDistance avoids a per-actor sqrt)
	forceDetectDistance *= forceDetectDistance;
	const float radiusSq = radius * radius;

	const auto sourceLocation = source->GetPosition();
	//Pass the plain radius to the scan; ForEachReferenceInRange expects a real-world distance, not a squared value
    Utilities::World::ForEachReferenceInRange(source, radius, [&](RE::TESObjectREFR& ref) {
		auto refBase = ref.GetBaseObject();
		auto actor = ref.As<RE::Actor>();
		if (actor && actor != source && !actor->IsDisabled() && !actor->IsChild() && !Utilities::Actor::IsDead(actor) && (ref.Is(RE::FormType::NPC) || (refBase && refBase->Is(RE::FormType::NPC)))) {
			//Enforce the outer radius as a hard cutoff, then check force distance / detection
			if (sourceLocation.GetSquaredDistance(ref.GetPosition()) < radiusSq &&
				(sourceLocation.GetSquaredDistance(ref.GetPosition()) < forceDetectDistance || (actor->RequestDetectionLevel(source, RE::DETECTION_PRIORITY::kNormal) > 0) || actor->IsPlayer())) {
				nearbyActors.push_back(actor);
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});

	return nearbyActors;
}



RE::BSEventNotifyControl RuntimeEvents::OnModCallbackEvent::ProcessEvent(const SKSE::ModCallbackEvent* callbackEvent, RE::BSTEventSource<SKSE::ModCallbackEvent>*)
{
	if (!callbackEvent) {
		return RE::BSEventNotifyControl::kContinue;
	}

	auto eventName = callbackEvent->eventName.c_str();
	if (!eventName || !std::strcmp(eventName, "OSLA_ANDUpdate") == 0) {
		return RE::BSEventNotifyControl::kContinue;
	}

	SKSE::log::debug("OnModCallbackEvent: Received ModCallbackEvent: EventName: {}", eventName);

	// Check if AND integration is enabled
	if (Settings::GetSingleton()->GetUseANDIntegration() && Integrations::ANDIntegration::GetSingleton()->IsAvailable()) {
		SKSE::log::debug("Processing OSLA_ANDUpdate: AND factions recalculated, triggering arousal recalculation");

		// Get the arousal system (only OSL mode supports libido modifier cache)
		auto& arousalSystem = ArousalManager::GetSingleton()->GetArousalSystem();
		if (arousalSystem.GetMode() == IArousalSystem::ArousalMode::kOSL) {
			auto* oslSystem = static_cast<ArousalSystemOSL*>(&arousalSystem);

			// Get player and nearby actors to update their arousal
			auto player = RE::PlayerCharacter::GetSingleton();
			if (player) {
				// Update player's libido cache
				oslSystem->ActorLibidoModifiersUpdated(player);
				float newBaseline = oslSystem->GetBaselineArousal(player); // Force baseline recalculation
				SKSE::log::trace("OSLA_ANDUpdate: Updated player libido cache, new baseline arousal: {}", newBaseline);

				// Update all nearby actors' libido cache
				const auto nearbyActors = GetNearbyActorsInCell(player);
				for (const auto& actorHandle : nearbyActors) {
					auto actorPtr = actorHandle.get();
					if (actorPtr) {
						auto actor = actorPtr.get();
						if (actor && ActorStateManager::GetSingleton()->IsHumanoidActor(actor)) {
							oslSystem->ActorLibidoModifiersUpdated(actor);
						}
					}
				}

				SKSE::log::debug("OSLA_ANDUpdate: Updated libido cache for {} nearby actors", nearbyActors.size());
			}
		}
	}

	return RE::BSEventNotifyControl::kContinue;
}
