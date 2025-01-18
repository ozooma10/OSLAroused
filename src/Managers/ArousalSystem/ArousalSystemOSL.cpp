#include "ArousalSystemOSL.h"
#include "PersistedData.h"
#include "Settings.h"
#include "Utilities/Utils.h"
#include "Papyrus/Papyrus.h"
#include "Integrations/DevicesIntegration.h"
#include "Managers/ArousalManager.h"

using namespace PersistedData;

float CalculateArousal(RE::Actor* actorRef, float gameHoursPassed)
{
    float currentArousal = ArousalData::GetSingleton()->GetData(actorRef->formID, -2.f);

    //If never calculated, regen
    if (currentArousal < -1) {
        currentArousal = Utilities::GenerateRandomFloat(10.f, 50.f);
        //logger::debug("Random Arousal: {} Val: {}", actorRef->GetDisplayFullName(), currentArousal);
        return currentArousal;
    }

    float currentArousalBaseline = ArousalManager::GetSingleton()->GetArousalSystem().GetBaselineArousal(actorRef);

    float epsilon = Settings::GetSingleton()->GetArousalChangeRate();
    //logger::trace("CalculateArousal: epsilon: {}", epsilon);


    float t = 1.f - std::pow(epsilon, gameHoursPassed);
    float newArousal = std::lerp(currentArousal, currentArousalBaseline, t);
    //logger::trace("CalculateArousal: {} from: {} newArousal {} Diff: {}  t: {}", actorRef->GetDisplayFullName(), currentArousal, newArousal, newArousal - currentArousal, t);

    return newArousal;
}

float ArousalSystemOSL::GetArousal(RE::Actor* actorRef, bool bUpdateState)
{
    if (!actorRef) {
        return -2.f;
    }

    RE::FormID actorFormId = actorRef->formID;

    const auto LastCheckTimeData = LastCheckTimeData::GetSingleton();
    auto lastCheckTime = LastCheckTimeData->GetData(actorFormId, 0.f);
    float curTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
    float gameHoursPassed = (curTime - lastCheckTime) * 24;

    float newArousal = CalculateArousal(actorRef, gameHoursPassed);

    //If set to update state, or we have never checked (last check time is 0), then update the lastchecktime
    if (bUpdateState || lastCheckTime == 0.f) {
        Utilities::Factions::SetFactionRank(actorRef, "sla_Arousal", newArousal);

        LastCheckTimeData->SetData(actorFormId, curTime);
        SetArousal(actorRef, newArousal, true);

        UpdateActorLibido(actorRef, gameHoursPassed, newArousal);
    }


    ActorStateManager::GetSingleton()->OnActorArousalUpdated(actorRef, newArousal);


    //logger::debug("Got Arousal for {} val: {}", actorRef->GetDisplayFullName(), newArousal);
    return newArousal;
}

float ArousalSystemOSL::SetArousal(RE::Actor* actorRef, float value, bool bSendEvent)
{
    value = std::clamp(value, 0.0f, 100.f);
    ArousalData::GetSingleton()->SetData(actorRef->formID, value);

    if (bSendEvent) {
        Papyrus::Events::SendActorArousalUpdatedEvent(actorRef, value);
    }

    return value;
}

float ArousalSystemOSL::ModifyArousal(RE::Actor* actorRef, float value, bool bSendEvent)
{
    value *= PersistedData::ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);
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

float ArousalSystemOSL::UpdateActorLibido(RE::Actor* actorRef, float gameHoursPassed, float targetLibido)
{
    //Move base libido towards targetlibido
    float epsilon = Settings::GetSingleton()->GetLibidoChangeRate();
    float currentVal = GetLibido(actorRef);

    //After 1 game hour, distance from curent to target is 10% closer 
    float t = 1.f - pow(epsilon, gameHoursPassed);
    float newVal = std::lerp(currentVal, targetLibido, t);
    //logger::trace("UpdateActorLibido: Lerped MOd from {} to {} DIFF: {}  t: {}", currentVal, newVal, newVal - currentVal, t);

    return SetLibido(actorRef, newVal);
}


float CalculateActorLibidoModifier(RE::Actor* actorRef)
{
    //Check if in scene
    //Check if near scene
    //Check Clothing
    //Check Devices
    const auto settings = Settings::GetSingleton();

    float libidoModifier = 0.f;
    bool isNaked = Utilities::Actor::IsNakedCached(actorRef);
    if (isNaked)
    {
        libidoModifier += settings->GetNudeArousalBaseline();
    }
    else if (Utilities::Actor::IsViewingNaked(actorRef)) {
        libidoModifier += settings->GetNudeViewingBaseline();
    }

    if (Utilities::Actor::IsParticipatingInScene(actorRef)) {
        libidoModifier += settings->GetSceneParticipantBaseline();
    }
    else if (Utilities::Actor::IsViewingScene(actorRef)) {
        libidoModifier += settings->GetSceneViewingBaseline();
    }

    if (!isNaked) {
        if (const auto eroticKeyword = settings->GetEroticArmorKeyword()) {
            const auto wornKeywords = Utilities::Actor::GetWornArmorKeywords(actorRef);
            if (wornKeywords.contains(eroticKeyword->formID)) {
                libidoModifier += settings->GetEroticArmorBaseline();
            }
        }
    }

    float deviceGain = DevicesIntegration::GetSingleton()->GetArousalBaselineFromDevices(actorRef);
    libidoModifier += deviceGain;
    return std::clamp(libidoModifier, 0.f, 100.f);
}