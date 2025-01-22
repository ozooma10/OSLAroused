#pragma once

#include "IArousalSystem.h"

class ArousalSystemSLA : public IArousalSystem
{
public:
	ArousalSystemSLA() {
		m_Mode = ArousalMode::kSLA;
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

	void HandleSpectatingNaked(RE::Actor* actorRef, RE::Actor* nakedRef) override;
};