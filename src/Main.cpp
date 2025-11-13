#include "PersistedData.h"

#include "Papyrus/Papyrus.h"
#include "Papyrus/PapyrusActor.h"
#include "Papyrus/PapyrusConfig.h"
#include "Papyrus/PapyrusInterface.h"

#include "RuntimeEvents.h"
#include "Utilities/Utils.h"
#include "Config.h"
#include "Managers/ArousalManager.h"
#include "Integrations/ANDIntegration.h"

using namespace RE::BSScript;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace
{
	void InitializeLogging()
	{
		auto path = log_directory();
		if (!path)
		{
			report_and_fail("Unable to lookup SKSE logs directory.");
		}
		*path /= SKSE::PluginDeclaration::GetSingleton()->GetName();
		*path += L".log";

		std::shared_ptr<spdlog::logger> log;
		if (IsDebuggerPresent())
		{
			log = std::make_shared<spdlog::logger>(
				"Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
		}
		else
		{
			log = std::make_shared<spdlog::logger>(
				"Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
		}
		log->set_level(spdlog::level::debug);
		log->flush_on(spdlog::level::trace);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
	}

	void InitializeSerialization()
	{
		SKSE::log::trace("Initializing cosave serialization...");
		auto* serialization = SKSE::GetSerializationInterface();
		serialization->SetUniqueID(PersistedData::kArousalDataKey);
		serialization->SetSaveCallback(PersistedData::SaveCallback);
		serialization->SetRevertCallback(PersistedData::RevertCallback);
		serialization->SetLoadCallback(PersistedData::LoadCallback);
		SKSE::log::trace("Cosave serialization initialized.");
	}

	void InitializePapyrus()
	{
		SKSE::log::trace("Initializing Papyrus binding...");
		const auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(Papyrus::RegisterFunctions);
		papyrus->Register(PapyrusInterface::RegisterFunctions);
		papyrus->Register(PapyrusConfig::RegisterFunctions);
		papyrus->Register(PapyrusActor::RegisterFunctions);
	}

	void InitializeMessaging() 
	{
		if (!SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
			switch (message->type) {
			case SKSE::MessagingInterface::kDataLoaded:  // All ESM/ESL/ESP plugins have loaded, main menu is now active
				RuntimeEvents::OnEquipEvent::RegisterEvent();
				Config::GetSingleton()->LoadINIs();
				Utilities::Factions::GetSingleton()->Initialize();
				// Initialize A.N.D. Integration after config is loaded
				Integrations::ANDIntegration::GetSingleton()->Initialize();
				WorldChecks::ArousalUpdateTicker::GetSingleton()->Start();
				break;
			case SKSE::MessagingInterface::kNewGame:
				//On new game, reset arousal manager
				ArousalManager::GetSingleton()->SetArousalSystem(IArousalSystem::ArousalMode::kOSL, false);
				break;
			case SKSE::MessagingInterface::kPostLoadGame:
				//Distribute Persisted Keywords
				Utilities::Keywords::DistributeKeywords();

				//Loaded game so update arousal mode based off saved data
				auto arousalMode = (IArousalSystem::ArousalMode)PersistedData::SettingsData::GetSingleton()->GetArousalMode();
				ArousalManager::GetSingleton()->SetArousalSystem(arousalMode, false);
				break;
			} }
		))
		{
			SKSE::stl::report_and_fail("Unable to register message listener.");
		}
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) 
{
	InitializeLogging();

	auto* plugin = SKSE::PluginDeclaration::GetSingleton();
	auto version = plugin->GetVersion();
	SKSE::log::info("{} {} is loading...", plugin->GetName(), version);

	SKSE::Init(skse);

	InitializeMessaging();
	InitializeSerialization();
	InitializePapyrus();

	SKSE::log::info("{} has finished loading.", plugin->GetName());
	return true;
}
