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

	class OnModCallbackEvent : public RE::BSTEventSink<SKSE::ModCallbackEvent>
	{
		virtual RE::BSEventNotifyControl ProcessEvent(const SKSE::ModCallbackEvent* callbackEvent, RE::BSTEventSource<SKSE::ModCallbackEvent>*) override;

	public:
		static bool RegisterEvent() {
			static OnModCallbackEvent g_ModCallbackEventHandler;
			auto modCallbackEventSource = SKSE::GetModCallbackEventSource();
			if(!modCallbackEventSource) {
				REX::ERROR("Failed to get ModCallbackEventSource for OnModCallbackEvent registration");
				return false;
			}

			modCallbackEventSource->AddEventSink(&g_ModCallbackEventHandler);
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

		// Baselined lazily on the first update tick (see RunWorldArousalUpdate in
		// RuntimeEvents.cpp). A negative sentinel means "not yet baselined"; this
		// also avoids touching RE::Calendar (null at the main menu) during the
		// singleton's construction at kDataLoaded.
		float LastUpdatePollGameTime = -1.f;

		float LastNearbyArousalUpdateGameTime = -1.f;

		std::vector<RE::ActorHandle> LastScannedActors;
	};
}
