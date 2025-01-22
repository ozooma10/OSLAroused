#pragma once

class IArousalSystem {
public:
	enum class ArousalMode
	{
		kOSL,
		kSLA
	};

	virtual float GetArousal(RE::Actor* actorRef, bool bUpdateState = true) = 0;

	//NOTE: In SLA Mode, this is the same as SetExposure/ModifyExposure 
	virtual float SetArousal(RE::Actor* actorRef, float value, bool bSendEvent) = 0;
	virtual float ModifyArousal(RE::Actor* actorRef, float value, bool bSendEvent) = 0;

	//This is primarily for SLA Mode since need to get exposure value separate from arousal
	virtual float GetExposure(RE::Actor* actorRef) = 0;

	//Libido is TimeRate in SLA Mode
	virtual float GetLibido(RE::Actor* actorRef) = 0;
	virtual float SetLibido(RE::Actor* actorRef, float value) = 0;
	virtual float ModifyLibido(RE::Actor* actorRef, float value) = 0;

	//ArousalMultipler is ExposureRate in SLA Mode
	virtual float GetArousalMultiplier(RE::Actor* actorRef) = 0;
	virtual float SetArousalMultiplier(RE::Actor* actorRef, float value) = 0;
	virtual float ModifyArousalMultiplier(RE::Actor* actorRef, float value) = 0;

	//OSL Mode Specific
	virtual float GetBaselineArousal(RE::Actor* actorRef) = 0;

	//SLA Mode Specific
	virtual void HandleSpectatingNaked(RE::Actor* actorRef, RE::Actor* nakedRef, float elapsedGameTimeSinceLastUpdate) = 0;

	virtual ~IArousalSystem() = default;

	ArousalMode GetMode() const { return m_Mode; }

protected:
	ArousalMode m_Mode;
};
