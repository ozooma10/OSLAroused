#pragma once
#include "IArousalSystem.h"
#include "Utilities/LRUCache.h"

float CalculateActorLibidoModifier(RE::Actor* actorFormId);

class ArousalSystemOSL : public IArousalSystem
{
public:
	ArousalSystemOSL() : m_LibidoModifierCache(std::function<float(RE::Actor*)>(CalculateActorLibidoModifier), 100) {
		m_Mode = ArousalMode::kOSL;
	}

	// Inherited via IArousalSystem
	float GetArousal(RE::Actor* actorRef, bool bUpdateState) override;
	float SetArousal(RE::Actor* actorRef, float value, bool bSendEvent) override;
	float ModifyArousal(RE::Actor* actorRef, float value, bool bSendEvent) override;

	float GetExposure(RE::Actor* actorRef) override;

	float GetLibido(RE::Actor* actorRef) override;
	float SetLibido(RE::Actor* actorRef, float value) override;
	float ModifyLibido(RE::Actor* actorRef, float value) override;

	float GetArousalMultiplier(RE::Actor* actorRef) override;
	float SetArousalMultiplier(RE::Actor* actorRef, float value) override;
	float ModifyArousalMultiplier(RE::Actor* actorRef, float value) override;

	float GetBaselineArousal(RE::Actor* actorRef) override;
	
	void ActorLibidoModifiersUpdated(RE::Actor* actorRef);
private:

	float UpdateActorLibido(RE::Actor* actorRef, float gameHoursPassed, float targetLibido);

	//Cache of additional Arousal values on top of actors base libido (Resulting in Baseline Arousal)
	Utilities::LRUCache<RE::Actor*, float> m_LibidoModifierCache;


	// Inherited via IArousalSystem
	void HandleSpectatingNaked(RE::Actor* actorRef, RE::Actor* nakedRef) override;

};