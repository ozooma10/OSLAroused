#include "PapyrusConfig.h"
#include "Config.h"
#include "Settings.h"
#include <Utilities/Utils.h>
#include "Managers/ArousalManager.h"
#include "Managers/ArousalSystem/ArousalSystemOSL.h"
#include "PersistedData.h"
#include "Integrations/ANDIntegration.h"

void PapyrusConfig::SetMinLibidoValue(RE::StaticFunctionTag*, bool bPlayerVal, float newVal)
{
	SKSE::log::trace("SetMinLibidoValue: {} {}", bPlayerVal, newVal);
	Settings::GetSingleton()->SetMinLibidoValue(bPlayerVal, newVal);
}

void PapyrusConfig::SetArousalChangeRate(RE::StaticFunctionTag*, float newVal)
{
	SKSE::log::trace("SetArousalChangeRate: {}", newVal);
	Settings::GetSingleton()->SetArousalChangeRate(newVal);
}

void PapyrusConfig::SetLibidoChangeRate(RE::StaticFunctionTag*, float newVal)
{
	SKSE::log::trace("SetLibidoChangeRate: {}", newVal);
	Settings::GetSingleton()->SetLibidoChangeRate(newVal);
}

void PapyrusConfig::SetSceneParticipantBaseline(RE::StaticFunctionTag*, float newVal)
{
	SKSE::log::trace("SetSceneParticipantBaseline: {}", newVal);
	Settings::GetSingleton()->SetSceneParticipantBaseline(newVal);

	// Clear cache when scene baseline changes
	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ClearAllLibidoModifiers();
	}
}

void PapyrusConfig::SetSceneViewingBaseline(RE::StaticFunctionTag*, float newVal)
{
	SKSE::log::trace("SetSceneViewingBaseline: {}", newVal);
	Settings::GetSingleton()->SetSceneViewingBaseline(newVal);

	// Clear cache when scene baseline changes
	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ClearAllLibidoModifiers();
	}
}

void PapyrusConfig::SetSceneVictimGainsArousal(RE::StaticFunctionTag*, bool newVal)
{
	SKSE::log::trace("SetSceneVictimGainsArousal: {}", newVal);
	Settings::GetSingleton()->SetSceneVictimGainsArousal(newVal);
}

void PapyrusConfig::SetBeingNudeBaseline(RE::StaticFunctionTag*, float newVal)
{
	SKSE::log::trace("SetBeingNudeBaseline: {}", newVal);
	Settings::GetSingleton()->SetNudeArousalBaseline(newVal);

	// Clear cache when nude baseline changes
	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ClearAllLibidoModifiers();
	}
}

void PapyrusConfig::SetViewingNudeBaseline(RE::StaticFunctionTag*, float newVal)
{
	SKSE::log::trace("SetViewingNudeBaseline: {}", newVal);
	Settings::GetSingleton()->SetNudeViewingBaseline(newVal);

	// Clear cache when nude viewing baseline changes
	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ClearAllLibidoModifiers();
	}
}

void PapyrusConfig::SetEroticArmorBaseline(RE::StaticFunctionTag*, float newVal, RE::BGSKeyword* keyword)
{
    if (!keyword) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return;
    }
	SKSE::log::trace("SetEroticArmorBaseline: {} {}", newVal, keyword->formID);

	Settings::GetSingleton()->SetEroticArmorBaseline(newVal, keyword);
	Config::GetSingleton()->SaveKeywordBaseline(keyword->formID, newVal);

	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ClearAllLibidoModifiers();
	}
}

float PapyrusConfig::GetEroticArmorBaseline(RE::StaticFunctionTag*, RE::BGSKeyword* keyword)
{
	if (!keyword) {
		Utilities::logInvalidArgsVerbose(__FUNCTION__);
		return 0.0f;
	}

	const auto baseline = Settings::GetSingleton()->GetEroticArmorBaseline(keyword);
	SKSE::log::trace("GetEroticArmorBaseline: {} {}", keyword->formID, baseline);
	return baseline;
}

// A.N.D. Integration functions
void PapyrusConfig::SetUseANDIntegration(RE::StaticFunctionTag*, bool enabled)
{
	SKSE::log::trace("SetUseANDIntegration: {}", enabled);
	Settings::GetSingleton()->SetUseANDIntegration(enabled);
}

bool PapyrusConfig::GetUseANDIntegration(RE::StaticFunctionTag*)
{
	bool result = Settings::GetSingleton()->GetUseANDIntegration();
	SKSE::log::trace("GetUseANDIntegration: {}", result);
	return result;
}

bool PapyrusConfig::IsANDIntegrationEnabled(RE::StaticFunctionTag*)
{
	bool result = Integrations::ANDIntegration::GetSingleton()->IsAvailable() && Settings::GetSingleton()->GetUseANDIntegration();
	return result;
}

void PapyrusConfig::SetSOSIntegrationEnabled(RE::StaticFunctionTag*, bool enabled)
{
	SKSE::log::trace("SetSOSIntegrationEnabled: {}", enabled);
	Settings::GetSingleton()->SetEnableSOSIntegration(enabled);
}

void PapyrusConfig::SetANDFactionBaseline(RE::StaticFunctionTag*, int factionIndex, float value)
{
	SKSE::log::trace("SetANDFactionBaseline: index={}, value={}", factionIndex, value);
	Settings::GetSingleton()->SetANDFactionBaseline(factionIndex, value);

	// Clear libido modifier cache for OSL mode when A.N.D. baselines change
	// This ensures all actors recalculate their baselines with the new settings
	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ClearAllLibidoModifiers();
	}
}

float PapyrusConfig::GetANDFactionBaseline(RE::StaticFunctionTag*, int factionIndex)
{
	float result = Settings::GetSingleton()->GetANDFactionBaseline(factionIndex);
	SKSE::log::trace("GetANDFactionBaseline: index={}, result={}", factionIndex, result);
	return result;
}

void PapyrusConfig::SetDeviceTypesBaseline1(RE::StaticFunctionTag*, float belt, float collar, float legCuffs, float armCuffs, float bra, float gag, float piercingsNipple, float piercingsVaginal, float blindfold, float harness)
{
	DeviceArousalBaselineChange arousalBaselineConfig = Settings::GetSingleton()->GetDeviceBaseline();
	arousalBaselineConfig.Belt = belt;
	arousalBaselineConfig.Collar = collar;
	arousalBaselineConfig.LegCuffs = legCuffs;
	arousalBaselineConfig.ArmCuffs = armCuffs;
	arousalBaselineConfig.Bra = bra;
	arousalBaselineConfig.Gag = gag;
	arousalBaselineConfig.PiercingsNipple = piercingsNipple;
	arousalBaselineConfig.PiercingsVaginal = piercingsVaginal;
	arousalBaselineConfig.Blindfold = blindfold;
	arousalBaselineConfig.Harness = harness;
	Settings::GetSingleton()->SetDeviceBaseline(arousalBaselineConfig);
}

void PapyrusConfig::SetDeviceTypesBaseline2(RE::StaticFunctionTag*, float plugVag, float plugAnal, float corset, float boots, float gloves, float hood, float suit, float heavyBondage, float bondageMittens)
{
	DeviceArousalBaselineChange arousalBaselineConfig = Settings::GetSingleton()->GetDeviceBaseline();
	arousalBaselineConfig.PlugVaginal = plugVag;
	arousalBaselineConfig.PlugAnal = plugAnal;
	arousalBaselineConfig.Corset = corset;
	arousalBaselineConfig.Boots = boots;
	arousalBaselineConfig.Gloves = gloves;
	arousalBaselineConfig.Hood = hood;
	arousalBaselineConfig.Suit = suit;
	arousalBaselineConfig.HeavyBondage = heavyBondage;
	arousalBaselineConfig.BondageMittens = bondageMittens;
	Settings::GetSingleton()->SetDeviceBaseline(arousalBaselineConfig);
}

void PapyrusConfig::SetDeviceTypeBaseline(RE::StaticFunctionTag*, int deviceTypeId, float newVal)
{
	DeviceArousalBaselineChange arousalBaselineConfig = Settings::GetSingleton()->GetDeviceBaseline();
	switch (deviceTypeId) {
	case DeviceType::Belt:
		arousalBaselineConfig.Belt = newVal;
		break;
	case DeviceType::Collar:
		arousalBaselineConfig.Collar = newVal;
		break;
	case DeviceType::LegCuffs:
		arousalBaselineConfig.LegCuffs = newVal;
		break;
	case DeviceType::ArmCuffs:
		arousalBaselineConfig.ArmCuffs = newVal;
		break;
	case DeviceType::Bra:
		arousalBaselineConfig.Bra = newVal;
		break;
	case DeviceType::Gag:
		arousalBaselineConfig.Gag = newVal;
		break;
	case DeviceType::PiercingsNipple:
		arousalBaselineConfig.PiercingsNipple = newVal;
		break;
	case DeviceType::PiercingsVaginal:
		arousalBaselineConfig.PiercingsVaginal = newVal;
		break;
	case DeviceType::Blindfold:
		arousalBaselineConfig.Blindfold = newVal;
		break;
	case DeviceType::Harness:
		arousalBaselineConfig.Harness = newVal;
		break;
	case DeviceType::PlugVaginal:
		arousalBaselineConfig.PlugVaginal = newVal;
		break;
	case DeviceType::PlugAnal:
		arousalBaselineConfig.PlugAnal = newVal;
		break;
	case DeviceType::Corset:
		arousalBaselineConfig.Corset = newVal;
		break;
	case DeviceType::Boots:
		arousalBaselineConfig.Boots = newVal;
		break;
	case DeviceType::Gloves:
		arousalBaselineConfig.Gloves = newVal;
		break;
	case DeviceType::Hood:
		arousalBaselineConfig.Hood = newVal;
		break;
	case DeviceType::Suit:
		arousalBaselineConfig.Suit = newVal;
		break;
	case DeviceType::HeavyBondage:
		arousalBaselineConfig.HeavyBondage = newVal;
		break;
	case DeviceType::BondageMittens:
		arousalBaselineConfig.BondageMittens = newVal;
		break;
	}
	Settings::GetSingleton()->SetDeviceBaseline(arousalBaselineConfig);
}

bool PapyrusConfig::IsInOSLMode(RE::StaticFunctionTag* base)
{
	auto arousalMode = ArousalManager::GetSingleton()->GetArousalSystem().GetMode();
	return arousalMode == IArousalSystem::ArousalMode::kOSL;
}

void PapyrusConfig::SetInOSLMode(RE::StaticFunctionTag* base, bool newVal)
{
	SKSE::log::trace("SetInOSLMode: {}", newVal);
	PersistedData::SettingsData::GetSingleton()->SetArousalMode((int)(newVal ? IArousalSystem::ArousalMode::kOSL : IArousalSystem::ArousalMode::kSLA));
	ArousalManager::GetSingleton()->SetArousalSystem(newVal ? IArousalSystem::ArousalMode::kOSL : IArousalSystem::ArousalMode::kSLA, true);
}

void PapyrusConfig::SetSLATimeRateHalfLife(RE::StaticFunctionTag* base, float newVal)
{
	SKSE::log::trace("SetSLATimeRateHalfLife: {}", newVal);
	Settings::GetSingleton()->SetTimeRateHalfLife(newVal);
}

void PapyrusConfig::SetSLADefaultExposureRate(RE::StaticFunctionTag* base, float newVal)
{
	SKSE::log::trace("SetSLADefaultExposureRate: {}", newVal);
	Settings::GetSingleton()->SetDefaultExposureRate(newVal);
}

float PapyrusConfig::GetUpdateIntervalRealTimeSeconds(RE::StaticFunctionTag* base)
{
	return Utilities::GameTimeToRealSeconds(Settings::GetSingleton()->GetArousalUpdateInterval());
}

void PapyrusConfig::SetSleepArousalGain(RE::StaticFunctionTag* base, float newVal)
{
	SKSE::log::trace("SetSleepArousalGain: {}", newVal);
	Settings::GetSingleton()->SetSleepArousalGain(newVal);
	Config::GetSingleton()->SaveSleepArousalGain(newVal);
}

float PapyrusConfig::GetSleepArousalGain(RE::StaticFunctionTag* base)
{
	return Settings::GetSingleton()->GetSleepArousalGain();
}

void PapyrusConfig::SetSpectatorArousalGain(RE::StaticFunctionTag* base, float newVal)
{
	SKSE::log::trace("SetSpectatorArousalGain: {}", newVal);
	Settings::GetSingleton()->SetSpectatorArousalGain(newVal);
	Config::GetSingleton()->SaveSpectatorArousalGain(newVal);
}

float PapyrusConfig::GetSpectatorArousalGain(RE::StaticFunctionTag* base)
{
	return Settings::GetSingleton()->GetSpectatorArousalGain();
}

RE::BSFixedString PapyrusConfig::RoundFloat(RE::StaticFunctionTag* base, float value, int decimals)
{
	// Clamp decimals to something sane
 	if (decimals < 0) {
		decimals = 0;
	} else if (decimals > 6) {
		decimals = 6;
	}

	std::stringstream ss;
	ss << std::fixed << std::setprecision(decimals) << value;

	// Convert std::string → BSFixedString
	return RE::BSFixedString(ss.str());
}

bool PapyrusConfig::RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
{
	vm->RegisterFunction("SetMinLibidoValue", "OSLArousedNativeConfig", SetMinLibidoValue);
	vm->RegisterFunction("SetArousalChangeRate", "OSLArousedNativeConfig", SetArousalChangeRate);
	vm->RegisterFunction("SetLibidoChangeRate", "OSLArousedNativeConfig", SetLibidoChangeRate);

	vm->RegisterFunction("SetSceneParticipantBaseline", "OSLArousedNativeConfig", SetSceneParticipantBaseline);
	vm->RegisterFunction("SetSceneViewingBaseline", "OSLArousedNativeConfig", SetSceneViewingBaseline);
	vm->RegisterFunction("SetSceneVictimGainsArousal", "OSLArousedNativeConfig", SetSceneVictimGainsArousal);
	vm->RegisterFunction("SetBeingNudeBaseline", "OSLArousedNativeConfig", SetBeingNudeBaseline);
	vm->RegisterFunction("SetViewingNudeBaseline", "OSLArousedNativeConfig", SetViewingNudeBaseline);

	vm->RegisterFunction("SetEroticArmorBaseline", "OSLArousedNativeConfig", SetEroticArmorBaseline);
	vm->RegisterFunction("GetEroticArmorBaseline", "OSLArousedNativeConfig", GetEroticArmorBaseline);

	// Register A.N.D. Integration functions
	vm->RegisterFunction("SetUseANDIntegration", "OSLArousedNativeConfig", SetUseANDIntegration);
	vm->RegisterFunction("GetUseANDIntegration", "OSLArousedNativeConfig", GetUseANDIntegration);
	vm->RegisterFunction("IsANDIntegrationEnabled", "OSLArousedNativeConfig", IsANDIntegrationEnabled);
	vm->RegisterFunction("SetANDFactionBaseline", "OSLArousedNativeConfig", SetANDFactionBaseline);
	vm->RegisterFunction("GetANDFactionBaseline", "OSLArousedNativeConfig", GetANDFactionBaseline);

	vm->RegisterFunction("SetDeviceTypesBaseline1", "OSLArousedNativeConfig", SetDeviceTypesBaseline1);
	vm->RegisterFunction("SetDeviceTypesBaseline2", "OSLArousedNativeConfig", SetDeviceTypesBaseline2);
	vm->RegisterFunction("SetDeviceTypeBaseline", "OSLArousedNativeConfig", SetDeviceTypeBaseline);

	vm->RegisterFunction("SetInOSLMode", "OSLArousedNativeConfig", SetInOSLMode);
	vm->RegisterFunction("IsInOSLMode", "OSLArousedNativeConfig", IsInOSLMode);

	vm->RegisterFunction("SetSLATimeRateHalfLife", "OSLArousedNativeConfig", SetSLATimeRateHalfLife);
	vm->RegisterFunction("SetSLADefaultExposureRate", "OSLArousedNativeConfig", SetSLADefaultExposureRate);

	vm->RegisterFunction("GetUpdateIntervalRealTimeSeconds", "OSLArousedNativeConfig", GetUpdateIntervalRealTimeSeconds);

	vm->RegisterFunction("SetSleepArousalGain", "OSLArousedNativeConfig", SetSleepArousalGain);
	vm->RegisterFunction("GetSleepArousalGain", "OSLArousedNativeConfig", GetSleepArousalGain);

	vm->RegisterFunction("SetSpectatorArousalGain", "OSLArousedNativeConfig", SetSpectatorArousalGain);
	vm->RegisterFunction("GetSpectatorArousalGain", "OSLArousedNativeConfig", GetSpectatorArousalGain);

	vm->RegisterFunction("SetSOSIntegrationEnabled", "OSLArousedNativeConfig", SetSOSIntegrationEnabled);

	vm->RegisterFunction("RoundFloat", "OSLArousedNativeConfig", RoundFloat);
	return true;
}
