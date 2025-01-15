#pragma once

#include "Managers/LibidoManager.h"

#include "ArousalSystem/IArousalSystem.h"
#include "ArousalSystem/ArousalSystemOSL.h"

namespace ArousalManager
{
	class SystemManager
	{
	public:
		static SystemManager* GetSingleton()
		{
			static SystemManager singleton;
			return &singleton;
		};

		SystemManager() : m_pArousalSystem(std::make_unique<ArousalSystemOSL>()) {};

		void SetArousalSystem(std::unique_ptr<IArousalSystem> pArousalSystem)
		{
			m_pArousalSystem = std::move(pArousalSystem);
		}

		IArousalSystem& GetArousalSystem()
		{
			return *m_pArousalSystem;
		}

	private:
		std::unique_ptr<IArousalSystem> m_pArousalSystem;
	};

	float GetArousal(RE::Actor* actorRef, bool bUpdateState = true);
	float SetArousal(RE::Actor* actorRef, float value);
	float ModifyArousal(RE::Actor* actorRef, float value);

	float CalculateArousal(RE::Actor* actorRef, float timePassed);

	//modified functions to allow paralel proc from different plugin
	extern "C" DLLEXPORT float GetArousalExt(RE::Actor* actorRef);
	extern "C" DLLEXPORT float SetArousalExt(RE::Actor* actorRef, float value, bool sendevent);
	extern "C" DLLEXPORT float ModifyArousalExt(RE::Actor* actorRef, float value,bool sendevent);
}
