#pragma once

#include "ArousalSystem/IArousalSystem.h"
#include "ArousalSystem/ArousalSystemOSL.h"
#include "ArousalSystem/ArousalSystemSLA.h"

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

	void SetArousalSystem(IArousalSystem::ArousalMode newMode)
	{
		logger::trace("ArosualManager::Setting Arousal System to {}", (int)newMode);
		if (newMode == IArousalSystem::ArousalMode::kOSL)
		{
			m_pArousalSystem = std::make_unique<ArousalSystemOSL>();
		}
		else
		{
			m_pArousalSystem = std::make_unique<ArousalSystemSLA>();
		}
	}

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
