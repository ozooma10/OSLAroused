#pragma once

class IArousalSystem {
public:
	virtual float GetArousal(RE::Actor* actorRef, bool bUpdateState = true) = 0;
	virtual float SetArousal(RE::Actor* actorRef, float value, bool bSendEvent) = 0;
	virtual float ModifyArousal(RE::Actor* actorRef, float value, bool bSendEvent) = 0;
};
