#pragma once

#include "ArousalSystem/IArousalSystem.h"

class ArousalManager
{
public:
	ArousalManager(const ArousalManager&) = delete;
	ArousalManager& operator=(const ArousalManager&) = delete;


	static ArousalManager* GetSingleton()
	{
		static ArousalManager singleton;
		return &singleton;
	};

	void SetArousalSystem(IArousalSystem::ArousalMode newMode, bool bResetData = true);

	IArousalSystem& GetArousalSystem() const
	{
		return *m_pArousalSystem;
	}

private:
	ArousalManager();

	std::unique_ptr<IArousalSystem> m_pArousalSystem;
};


namespace Arousal {
	float GetArousal(RE::Actor* actorRef, bool bUpdateState = true);
	float SetArousal(RE::Actor* actorRef, float value);
	float ModifyArousal(RE::Actor* actorRef, float value);

	//modified functions to allow paralel proc from different plugin
	extern "C" DLLEXPORT float GetArousalExt(RE::Actor* actorRef);
	extern "C" DLLEXPORT float SetArousalExt(RE::Actor* actorRef, float value, bool sendevent);
	extern "C" DLLEXPORT float ModifyArousalExt(RE::Actor* actorRef, float value,bool sendevent);
}
