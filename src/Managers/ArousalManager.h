#pragma once

#include "ArousalSystem/IArousalSystem.h"
#include "ArousalSystem/ArousalSystemOSL.h"
#include "ArousalSystem/ArousalSystemSLA.h"

namespace ArousalManager
{
	class ArousalManager
	{
	public:

		ArousalManager() : m_pArousalSystem(std::make_unique<ArousalSystemSLA>()) {};

		void SetArousalSystem(IArousalSystem::ArousalMode newMode)
		{
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
		std::unique_ptr<IArousalSystem> m_pArousalSystem;
	};

	static ArousalManager* GetSingleton()
	{
		static ArousalManager singleton;
		return &singleton;
	};

	float GetArousal(RE::Actor* actorRef, bool bUpdateState = true);
	float SetArousal(RE::Actor* actorRef, float value);
	float ModifyArousal(RE::Actor* actorRef, float value);

	//modified functions to allow paralel proc from different plugin
	extern "C" DLLEXPORT float GetArousalExt(RE::Actor* actorRef);
	extern "C" DLLEXPORT float SetArousalExt(RE::Actor* actorRef, float value, bool sendevent);
	extern "C" DLLEXPORT float ModifyArousalExt(RE::Actor* actorRef, float value,bool sendevent);
}
