#include "ArousalSystemOSL.h"
#include "PersistedData.h"
#include "Settings.h"
#include "Utilities/Utils.h"
#include "Managers/LibidoManager.h"
#include "Papyrus/Papyrus.h"

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

    float currentArousalBaseline = LibidoManager::GetSingleton()->GetBaselineArousal(actorRef);

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
        LastCheckTimeData->SetData(actorFormId, curTime);
        SetArousal(actorRef, newArousal);

        LibidoManager::GetSingleton()->UpdateActorLibido(actorRef, gameHoursPassed, newArousal);
    }
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