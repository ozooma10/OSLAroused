#pragma once
#include "Utilities/Ticker.h"

namespace RuntimeEvents
{
	class OnEquipEvent : public RE::BSTEventSink<RE::TESEquipEvent>
	{
		virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* equipEvent, RE::BSTEventSource<RE::TESEquipEvent>*) override;

	public:
		static bool RegisterEvent() {
			static OnEquipEvent g_EquipEventHandler;
			auto ScriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();
			if (!ScriptEventSource) {
				return false;
			}
			ScriptEventSource->AddEventSink(&g_EquipEventHandler);
			return true;
		}
	};
}

namespace WorldChecks
{
	void ArousalUpdateLoop();

	class ArousalUpdateTicker : public Utilities::Ticker
	{
	public:
        ArousalUpdateTicker(std::chrono::milliseconds interval)
            :
			Utilities::Ticker(std::function<void()>(ArousalUpdateLoop), interval) {}

		static ArousalUpdateTicker* GetSingleton()
		{
            static ArousalUpdateTicker singleton(std::chrono::milliseconds(15000));
			return &singleton;
		}

		float LastUpdatePollGameTime = RE::Calendar::GetSingleton()->GetHoursPassed();

		float LastNearbyArousalUpdateGameTime = RE::Calendar::GetSingleton()->GetHoursPassed();

		std::vector<RE::ActorHandle> LastScannedActors;
	};
}
