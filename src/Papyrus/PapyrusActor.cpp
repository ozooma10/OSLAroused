#include "PapyrusActor.h"
#include "Utilities/Utils.h"
#include "Integrations/DevicesIntegration.h"
#include "Settings.h"

std::vector<RE::TESForm*> PapyrusActor::GetAllEquippedArmor(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return Utilities::Actor::GetWornArmor(actorRef);
}

std::vector<int> PapyrusActor::GetActiveDeviceTypeIds(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	const auto wornDevices = DevicesIntegration::GetSingleton()->GetActorWornDevices(actorRef);

	std::vector<int> deviceTypeIds;

	if (wornDevices.ArmCuffs) {
		deviceTypeIds.push_back(DeviceType::ArmCuffs);
	}
	if (wornDevices.Belt) {
		deviceTypeIds.push_back(DeviceType::Belt);
	}
	if (wornDevices.Blindfold) {
		deviceTypeIds.push_back(DeviceType::Blindfold);
	}
	if (wornDevices.BondageMittens) {
		deviceTypeIds.push_back(DeviceType::BondageMittens);
	}
	if (wornDevices.Boots) {
		deviceTypeIds.push_back(DeviceType::Boots);
	}
	if (wornDevices.Bra) {
		deviceTypeIds.push_back(DeviceType::Bra);
	}
	if (wornDevices.Collar) {
		deviceTypeIds.push_back(DeviceType::Collar);
	}
	if (wornDevices.Corset) {
		deviceTypeIds.push_back(DeviceType::Corset);
	}
	if (wornDevices.Gag) {
		deviceTypeIds.push_back(DeviceType::Gag);
	}
	if (wornDevices.Gloves) {
		deviceTypeIds.push_back(DeviceType::Gloves);
	}
	if (wornDevices.Harness) {
		deviceTypeIds.push_back(DeviceType::Harness);
	}
	if (wornDevices.HeavyBondage) {
		deviceTypeIds.push_back(DeviceType::HeavyBondage);
	}
	if (wornDevices.Hood) {
		deviceTypeIds.push_back(DeviceType::Hood);
	}
	if (wornDevices.LegCuffs) {
		deviceTypeIds.push_back(DeviceType::LegCuffs);
	}
	if (wornDevices.PiercingsNipple) {
		deviceTypeIds.push_back(DeviceType::PiercingsNipple);
	}
	if (wornDevices.PiercingsVaginal) {
		deviceTypeIds.push_back(DeviceType::PiercingsVaginal);
	}
	if (wornDevices.PlugAnal) {
		deviceTypeIds.push_back(DeviceType::PlugAnal);
	}
	if (wornDevices.PlugVaginal) {
		deviceTypeIds.push_back(DeviceType::PlugVaginal);
	}
	if (wornDevices.Suit) {
		deviceTypeIds.push_back(DeviceType::Suit);
	}

	return deviceTypeIds;
}

bool PapyrusActor::RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
{
	vm->RegisterFunction("GetAllEquippedArmor", "OSLArousedNativeActor", GetAllEquippedArmor);
	vm->RegisterFunction("GetActiveDeviceTypeIds", "OSLArousedNativeActor", GetActiveDeviceTypeIds);
	return false;
}
