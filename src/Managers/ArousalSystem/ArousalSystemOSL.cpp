#include "ArousalSystemOSL.h"
#include "PersistedData.h"
#include "Settings.h"
#include "Utilities/Utils.h"
#include "Papyrus/Papyrus.h"
#include "Integrations/DevicesIntegration.h"
#include "Integrations/ANDIntegration.h"
#include "Integrations/ANDFactionIndices.h"
#include "Managers/ArousalManager.h"

using namespace PersistedData;

float CalculateArousal(RE::Actor* actorRef, float gameHoursPassed)
{
    float currentArousal = ArousalData::GetSingleton()->GetData(actorRef->formID, -2.f);

    //If never calculated, regen
    if (currentArousal < -1) {
        currentArousal = Utilities::GenerateRandomFloat(10.f, 50.f);
        //SKSE::log::debug("Random Arousal: {} Val: {}", actorRef->GetDisplayFullName(), currentArousal);
        return currentArousal;
    }

    float currentArousalBaseline = ArousalManager::GetSingleton()->GetArousalSystem().GetBaselineArousal(actorRef);

    float epsilon = Settings::GetSingleton()->GetArousalChangeRate();
    //SKSE::log::trace("CalculateArousal: epsilon: {}", epsilon);


    float t = 1.f - std::pow(epsilon, gameHoursPassed);
    float newArousal = std::lerp(currentArousal, currentArousalBaseline, t);
    //SKSE::log::trace("CalculateArousal: {} from: {} newArousal {} Diff: {}  t: {}", actorRef->GetDisplayFullName(), currentArousal, newArousal, newArousal - currentArousal, t);

    return newArousal;
}

float ArousalSystemOSL::GetArousal(RE::Actor* actorRef, bool bUpdateState)
{
    if (!actorRef || actorRef->IsChild()) {
        return 0.f;
    }

    if (Utilities::Actor::IsDead(actorRef)) {
        if (bUpdateState) {
            // Dead actors go flacid, drive the native SOS event. (noraml per-cycle skipped)
            ActorStateManager::GetSingleton()->UpdateSOSAnimation(actorRef, 0.0f);
        }
        return 0.0f;
    }
    
    //If locked, get the stored arousal value (which shouldnt be updated)
    if (ActorStateManager::GetActorArousalLocked(actorRef))
    {
		return ArousalData::GetSingleton()->GetData(actorRef->formID, 0.f);
    }

    RE::FormID actorFormId = actorRef->formID;

    const auto LastCheckTimeData = LastCheckTimeData::GetSingleton();
    auto lastCheckTime = LastCheckTimeData->GetData(actorFormId, 0.f);
    float curTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
    float gameHoursPassed = (curTime - lastCheckTime) * 24;

    float newArousal = CalculateArousal(actorRef, gameHoursPassed);

    //If set to update state, or we have never checked (last check time is 0), then update the lastchecktime
    if (bUpdateState || lastCheckTime == 0.f) {
        LastCheckTimeData->SetData(actorFormId, curTime);
        SetArousal(actorRef, newArousal, true);

        UpdateActorLibido(actorRef, gameHoursPassed, newArousal);
    }


    ActorStateManager::GetSingleton()->OnActorArousalUpdated(actorRef, newArousal);


    //SKSE::log::debug("Got Arousal for {} val: {}", actorRef->GetDisplayFullName(), newArousal);
    return newArousal;
}

float ArousalSystemOSL::SetArousal(RE::Actor* actorRef, float value, bool bSendEvent)
{
	if (!actorRef || actorRef->IsChild() || Utilities::Actor::IsDead(actorRef)) {
		return 0.0f;
	}

	if (ActorStateManager::GetActorArousalLocked(actorRef))
	{
		return ArousalData::GetSingleton()->GetData(actorRef->formID, 0.f);
	}

    value = std::clamp(value, 0.0f, 100.f);
    ArousalData::GetSingleton()->SetData(actorRef->formID, value);

	//OSL Mode sets both arousal and exposure to the same value
    Utilities::Factions::GetSingleton()->SetFactionRank(actorRef, FactionType::sla_Arousal, value);
    Utilities::Factions::GetSingleton()->SetFactionRank(actorRef, FactionType::sla_Exposure, value);

    if (bSendEvent) {
        // Drive SOS natively (deduped by bend bucket)
        // avoids round-tripping every actor through Papyrus OnActorArousalUpdated -> Debug.SendAnimationEvent.
        ActorStateManager::GetSingleton()->UpdateSOSAnimation(actorRef, value);

        // Only emit the public arousal event when it moved >= epsilon since the last notification (player always notifies)
        if (ActorStateManager::GetSingleton()->ShouldNotifyArousalChange(actorRef, value)) {
            Papyrus::Events::SendActorArousalUpdatedEvent(actorRef, value);
        }
    }

    return value;
}

float ArousalSystemOSL::ModifyArousal(RE::Actor* actorRef, float value, bool bSendEvent)
{
	if (!actorRef || actorRef->IsChild() || Utilities::Actor::IsDead(actorRef)) {
		return 0.0f;
	}

	float multiplier = PersistedData::ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);
	SKSE::log::trace("[{}] - ModifyArousal: {} * {} = {}", actorRef->GetDisplayFullName(), value, multiplier, value * multiplier);
    value *= multiplier;
    float currentArousal = GetArousal(actorRef, false);
    return SetArousal(actorRef, currentArousal + value, bSendEvent);
}

float ArousalSystemOSL::GetExposure(RE::Actor* actorRef)
{
    //Exposure is just arousal in OSL Mode
    return GetArousal(actorRef, true);
}

float ArousalSystemOSL::GetLibido(RE::Actor* actorRef)
{
	return BaseLibidoData::GetSingleton()->GetData(actorRef->formID, 0.f);
}

float ArousalSystemOSL::SetLibido(RE::Actor* actorRef, float value)
{
	value = std::clamp(value, Settings::GetSingleton()->GetMinLibidoValue(actorRef->IsPlayerRef()), 100.f);
	BaseLibidoData::GetSingleton()->SetData(actorRef->formID, value);
    return value;
}

float ArousalSystemOSL::ModifyLibido(RE::Actor* actorRef, float value)
{
	float curVal = GetLibido(actorRef);
    if (value == 0.f) {
        return curVal;
    }
	curVal += value;
	return SetLibido(actorRef, curVal);
}

float ArousalSystemOSL::GetArousalMultiplier(RE::Actor* actorRef)
{
	return ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);
}

float ArousalSystemOSL::SetArousalMultiplier(RE::Actor* actorRef, float value)
{
	value = std::clamp(value, 0.0f, 100.f);
	ArousalMultiplierData::GetSingleton()->SetData(actorRef->formID, value);
	return value;
}

float ArousalSystemOSL::ModifyArousalMultiplier(RE::Actor* actorRef, float value)
{
	float curMult = GetArousalMultiplier(actorRef);
	if (value == 0.f) {
		return curMult;
	}
	curMult += value;
	return SetArousalMultiplier(actorRef, curMult);
}

float ArousalSystemOSL::GetBaselineArousal(RE::Actor* actorRef)
{
    return std::max(m_LibidoModifierCache(actorRef), GetLibido(actorRef));
}

void ArousalSystemOSL::ActorLibidoModifiersUpdated(RE::Actor* actorRef)
{
	m_LibidoModifierCache.PurgeItem(actorRef);
}

void ArousalSystemOSL::ClearAllLibidoModifiers()
{
	SKSE::log::debug("OSL: Clearing all libido modifier caches (settings changed)");
	m_LibidoModifierCache.ClearAll();
}

float ArousalSystemOSL::UpdateActorLibido(RE::Actor* actorRef, float gameHoursPassed, float targetLibido)
{
    //Move base libido towards targetlibido
    float epsilon = Settings::GetSingleton()->GetLibidoChangeRate();
    float currentVal = GetLibido(actorRef);

    //After 1 game hour, distance from curent to target is 10% closer 
    float t = 1.f - std::pow(epsilon, gameHoursPassed);
    float newVal = std::lerp(currentVal, targetLibido, t);
    //SKSE::log::trace("UpdateActorLibido: Lerped MOd from {} to {} DIFF: {}  t: {}", currentVal, newVal, newVal - currentVal, t);

    return SetLibido(actorRef, newVal);
}

void ArousalSystemOSL::HandleSpectatingNaked(RE::Actor* actorRef, RE::Actor* nakedRef, float elapsedGameTimeSinceLastUpdate)
{
    if (!actorRef || !nakedRef) {
        return;
    }
    if (Utilities::Actor::IsDead(actorRef) || Utilities::Actor::IsDead(nakedRef)) {
        return;
    }

    // Player-focused effect. NPCs floor their libido high (default 80), so a crowd-wide
    // direct gain would just pin every nearby NPC near 100 in nude-heavy areas. Restrict
    // it to the player - either as the observer (actorRef) or, for the exhibitionist
    // branch, as the one being seen (nakedRef). Bailing here also skips the AND/faction
    // lookups below for the common NPC<->NPC pair, which is the bulk of the scan.
    if (!actorRef->IsPlayerRef() && !nakedRef->IsPlayerRef()) {
        return;
    }

    const auto settings = Settings::GetSingleton();

    // Direct, lasting arousal gain for witnessing nudity. This complements the
    // passive viewing-baseline lift in CalculateActorLibidoModifier: the baseline
    // makes the observer aroused *while* watching, this leaves a residue that
    // persists (and slowly normalizes) after they look away. 0 disables.
    const float baseGain = settings->GetSpectatorArousalGain();
    if (baseGain <= 0.f) {
        return;
    }

    // Scale by elapsed time (this is called every tick, not at the SLA cadence).
    // Reaches full gain at the configured update interval.
    const float updateInterval = settings->GetArousalUpdateInterval();
    const float timeScale = updateInterval > 0.f ? std::min(1.f, elapsedGameTimeSinceLastUpdate / updateInterval) : 1.f;

    // Scale by AND nudity level when available (partial nudity => partial gain).
    float nudityScale = 1.0f;
    if (settings->GetUseANDIntegration() && Integrations::ANDIntegration::GetSingleton()->IsAvailable()) {
        const float andScore = Integrations::ANDIntegration::GetSingleton()->GetANDNudityScore(nakedRef);
        const float maxNudeScore = settings->GetANDFactionBaseline(Integrations::ANDFactionIndex::NUDE);
        nudityScale = maxNudeScore > 0.f ? std::min(1.f, andScore / maxNudeScore) : 0.f;
    }

    // Observer (player) gain. Skipped while the player is participating in a scene: the
    // scene baselines/gains already drive arousal there, so this would double-dip.
    if (actorRef->IsPlayerRef() && !Utilities::Actor::IsParticipatingInScene(actorRef)) {
        // Full gain if the naked actor matches the observer's gender preference, else half.
        const int genderPreference = Utilities::Factions::GetSingleton()->GetFactionRank(actorRef, FactionType::sla_GenderPreference);
        const auto nakedBase = nakedRef->GetActorBase();
        const bool matchesPreference = nakedBase && (genderPreference == nakedBase->GetSex() || genderPreference == 2);

        const float gain = baseGain * timeScale * nudityScale * (matchesPreference ? 1.f : 0.5f);
        if (gain > 0.f) {
            //Main update loop runs GetArousal, so no need to send an event here
            ModifyArousal(actorRef, gain, false);
        }
    }

    // An exhibitionist player gains arousal from being seen (same player-only + not-in-scene gating).
    if (nakedRef->IsPlayerRef()
        && !Utilities::Actor::IsParticipatingInScene(nakedRef)
        && PersistedData::IsActorExhibitionistData::GetSingleton()->GetData(nakedRef->formID, false)) {
        const float exhibitionistGain = baseGain * timeScale * nudityScale * 0.5f;
        if (exhibitionistGain > 0.f) {
            ModifyArousal(nakedRef, exhibitionistGain, false);
        }
    }
}


float CalculateActorLibidoModifier(RE::Actor* actorRef)
{
    //Check if in scene
    //Check if near scene
    //Check Clothing
    //Check Devices
    const auto settings = Settings::GetSingleton();

    float libidoModifier = 0.f;

    // Use A.N.D. Integration for nudity-based arousal if available
    // This handles both A.N.D. nudity detection and legacy fallback (even if AND not available)
    float nudityModifier = Integrations::ANDIntegration::GetSingleton()->GetNudityBaselineModifier(actorRef);
    libidoModifier += nudityModifier;

    if (Utilities::Actor::IsViewingNaked(actorRef)) {
        float nudeViewingBaseline = settings->GetNudeViewingBaseline();

        // Scale the viewing baseline based on AND nudity score if enabled
        if (settings->GetUseANDIntegration() && Integrations::ANDIntegration::GetSingleton()->IsAvailable()) {
            float maxNudityScore = ActorStateManager::GetSingleton()->GetSpectatingMaxNudityScore(actorRef);
            if (maxNudityScore > 0.0f) {
                // Scale from 0.0 to 1.0 based on nudity score (configured Nude baseline = 1.0 scale)
                float maxNudeScore = settings->GetANDFactionBaseline(Integrations::ANDFactionIndex::NUDE);
                float nudityScale = maxNudeScore > 0.0f ? std::min(1.0f, maxNudityScore / maxNudeScore) : 0.0f;
                nudeViewingBaseline *= nudityScale;
                SKSE::log::trace("OSL: Actor {} viewing nudity scaled baseline: {} (score: {}, max: {}, scale: {})",
                             actorRef->GetDisplayFullName(), nudeViewingBaseline, maxNudityScore, maxNudeScore, nudityScale);
            }
        }

        libidoModifier += nudeViewingBaseline;
    }

    if (Utilities::Actor::IsParticipatingInScene(actorRef)) {
        libidoModifier += settings->GetSceneParticipantBaseline();
    }
    else if (Utilities::Actor::IsViewingScene(actorRef)) {
        libidoModifier += settings->GetSceneViewingBaseline();
    }

	// SKSE::log::trace("CalculateLibido for Actor: {} Base: {} nudityModifier: {} viewingNaked: {} Scene: {} SceneView: {} Erotic: {}",
		// actorRef->GetDisplayFullName(), libidoModifier, nudityModifier, Utilities::Actor::IsViewingNaked(actorRef), Utilities::Actor::IsParticipatingInScene(actorRef), Utilities::Actor::IsViewingScene(actorRef), settings->GetEroticArmorKeyword() ? settings->GetEroticArmorKeyword()->formID : 0);

    float deviceGain = DevicesIntegration::GetSingleton()->GetArousalBaselineFromDevices(actorRef);
    libidoModifier += deviceGain;
    return std::clamp(libidoModifier, 0.f, 100.f);
}
