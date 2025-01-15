#pragma once

#include "IArousalSystem.h"

class ArousalSystemOSL : public IArousalSystem
{
	// Inherited via IArousalSystem
	float GetArousal(RE::Actor* actorRef, bool bUpdateState) override;
	float SetArousal(RE::Actor* actorRef, float value, bool bSendEvent = true) override;
	float ModifyArousal(RE::Actor* actorRef, float value, bool bSendEvent = true) override;
};