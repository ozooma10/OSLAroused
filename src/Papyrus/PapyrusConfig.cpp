#include "PapyrusConfig.h"
#include "Settings.h"
#include <Utilities/Utils.h>
#include "Managers/ArousalManager.h"
#include "PersistedData.h"

void PapyrusConfig::SetMinLibidoValue(RE::StaticFunctionTag*, bool bPlayerVal, float newVal)
{
	logger::trace("SetMinLibidoValue: {} {}", bPlayerVal, newVal);
	Settings::GetSingleton()->SetMinLibidoValue(bPlayerVal, newVal);
}

void PapyrusConfig::SetArousalChangeRate(RE::StaticFunctionTag*, float newVal)
{
	logger::trace("SetArousalChangeRate: {}", newVal);
	Settings::GetSingleton()->SetArousalChangeRate(newVal);
}

void PapyrusConfig::SetLibidoChangeRate(RE::StaticFunctionTag*, float newVal)
{
	logger::trace("SetLibidoChangeRate: {}", newVal);
	Settings::GetSingleton()->SetLibidoChangeRate(newVal);
}

void PapyrusConfig::SetSceneParticipantBaseline(RE::StaticFunctionTag*, float newVal)
{
	logger::trace("SetSceneParticipantBaseline: {}", newVal);
	Settings::GetSingleton()->SetSceneParticipantBaseline(newVal);
}

void PapyrusConfig::SetSceneViewingBaseline(RE::StaticFunctionTag*, float newVal)
{
	logger::trace("SetSceneViewingBaseline: {}", newVal);
	Settings::GetSingleton()->SetSceneViewingBaseline(newVal);
}

void PapyrusConfig::SetSceneVictimGainsArousal(RE::StaticFunctionTag*, bool newVal)
{
	logger::trace("SetSceneVictimGainsArousal: {}", newVal);
	Settings::GetSingleton()->SetSceneVictimGainsArousal(newVal);
}

void PapyrusConfig::SetBeingNudeBaseline(RE::StaticFunctionTag*, float newVal)
{
	logger::trace("SetBeingNudeBaseline: {}", newVal);
	Settings::GetSingleton()->SetNudeArousalBaseline(newVal);
}

void PapyrusConfig::SetViewingNudeBaseline(RE::StaticFunctionTag*, float newVal)
{
	logger::trace("SetViewingNudeBaseline: {}", newVal);
	Settings::GetSingleton()->SetNudeViewingBaseline(newVal);
}

void PapyrusConfig::SetEroticArmorBaseline(RE::StaticFunctionTag*, float newVal, RE::BGSKeyword* keyword)
{
	logger::trace("SetEroticArmorBaseline: {} {}", newVal, keyword->formID);
    if (!keyword) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return;
    }
	Settings::GetSingleton()->SetEroticArmorBaseline(newVal, keyword);
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
		arousalBaselineConfig.Belt = newVal;
		break;
	case DeviceType::LegCuffs:
		arousalBaselineConfig.Belt = newVal;
		break;
	case DeviceType::ArmCuffs:
		arousalBaselineConfig.Belt = newVal;
		break;
	case DeviceType::Bra:
		arousalBaselineConfig.Belt = newVal;
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
	logger::trace("SetInOSLMode: {}", newVal);
	PersistedData::SettingsData::GetSingleton()->SetArousalMode((int)(newVal ? IArousalSystem::ArousalMode::kOSL : IArousalSystem::ArousalMode::kSLA));
	ArousalManager::GetSingleton()->SetArousalSystem(newVal ? IArousalSystem::ArousalMode::kOSL : IArousalSystem::ArousalMode::kSLA);
}

void PapyrusConfig::SetSLATimeRateHalfLife(RE::StaticFunctionTag* base, float newVal)
{
	logger::trace("SetSLATimeRateHalfLife: {}", newVal);
	Settings::GetSingleton()->SetTimeRateHalfLife(newVal);
}

void PapyrusConfig::SetSLADefaultExposureRate(RE::StaticFunctionTag* base, float newVal)
{
	logger::trace("SetSLADefaultExposureRate: {}", newVal);
	Settings::GetSingleton()->SetDefaultExposureRate(newVal);
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

	vm->RegisterFunction("SetDeviceTypesBaseline1", "OSLArousedNativeConfig", SetDeviceTypesBaseline1);
	vm->RegisterFunction("SetDeviceTypesBaseline2", "OSLArousedNativeConfig", SetDeviceTypesBaseline2);
	vm->RegisterFunction("SetDeviceTypeBaseline", "OSLArousedNativeConfig", SetDeviceTypeBaseline);

	vm->RegisterFunction("SetInOSLMode", "OSLArousedNativeConfig", SetInOSLMode);
	vm->RegisterFunction("IsInOSLMode", "OSLArousedNativeConfig", IsInOSLMode);

	vm->RegisterFunction("SetSLATimeRateHalfLife", "OSLArousedNativeConfig", SetSLATimeRateHalfLife);
	vm->RegisterFunction("SetSLADefaultExposureRate", "OSLArousedNativeConfig", SetSLADefaultExposureRate);
	
	return true;
}
