#include "LibidoManager.h"
#include "PersistedData.h"
#include "Utilities/Utils.h"
#include "Settings.h"
#include "Integrations/DevicesIntegration.h"

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
	} else if (Utilities::Actor::IsViewingNaked(actorRef)) {
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

float LibidoManager::UpdateActorLibido(RE::Actor* actorRef, float gameHoursPassed, float targetLibido)
{
	//Move base libido towards targetlibido
	float epsilon = Settings::GetSingleton()->GetLibidoChangeRate();
	float currentVal = GetBaseLibido(actorRef);

	//After 1 game hour, distance from curent to target is 10% closer 
	float t = 1.f - pow(epsilon, gameHoursPassed);
	float newVal = std::lerp(currentVal, targetLibido, t);
	//logger::trace("UpdateActorLibido: Lerped MOd from {} to {} DIFF: {}  t: {}", currentVal, newVal, newVal - currentVal, t);

	return SetBaseLibido(actorRef, newVal);
}

float LibidoManager::GetBaselineArousal(RE::Actor* actorRef)
{
	return std::max(m_LibidoModifierCache(actorRef), GetBaseLibido(actorRef));
}

float LibidoManager::GetBaseLibido(RE::Actor* actorRef)
{
	return PersistedData::BaseLibidoData::GetSingleton()->GetData(actorRef->formID, 0.f);
}

float LibidoManager::SetBaseLibido(RE::Actor* actorRef, float newVal)
{
	newVal = std::clamp(newVal, Settings::GetSingleton()->GetMinLibidoValue(actorRef->IsPlayerRef()), 100.f);
	
	PersistedData::BaseLibidoData::GetSingleton()->SetData(actorRef->formID, newVal);

	return newVal;
}

float LibidoManager::ModifyBaseLibido(RE::Actor* actorRef, float modVal)
{
	float curVal = PersistedData::BaseLibidoData::GetSingleton()->GetData(actorRef->formID, 0.f);
	if (modVal == 0.f) {
		return curVal;
	}
	curVal += modVal;

	return SetBaseLibido(actorRef, modVal);
}

void LibidoManager::ActorLibidoModifiersUpdated(RE::Actor* actorRef)
{
	m_LibidoModifierCache.PurgeItem(actorRef);
}
