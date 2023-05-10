#include "PapyrusInterface.h"

#include "PersistedData.h"
#include "Managers/ArousalManager.h"
#include "Managers/LibidoManager.h"
#include "Utilities/Utils.h"
#include <Settings.h>
#include <Integrations/DevicesIntegration.h>

float PapyrusInterface::GetArousal(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return ArousalManager::GetArousal(actorRef);
}

std::vector<float> PapyrusInterface::GetArousalMultiple(RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> actorRefs)
{
	std::vector<float> results;

	for (const auto actorRef : actorRefs) {
		results.push_back(ArousalManager::GetArousal(actorRef));
	}

	return results;
}

float PapyrusInterface::GetArousalNoSideEffects(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return ArousalManager::GetArousal(actorRef, false);
}

float PapyrusInterface::SetArousal(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	return ArousalManager::SetArousal(actorRef, value);
}

void PapyrusInterface::SetArousalMultiple(RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> actorRefs, float value)
{
	for (const auto actorRef : actorRefs) {
		ArousalManager::SetArousal(actorRef, value);
	}
}

float PapyrusInterface::ModifyArousal(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	return ArousalManager::ModifyArousal(actorRef, value);
}

void PapyrusInterface::ModifyArousalMultiple(RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> actorRefs, float value)
{
	for (const auto actorRef : actorRefs) {
		ArousalManager::ModifyArousal(actorRef, value);
	}
}

float PapyrusInterface::GetArousalMultiplier(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return PersistedData::ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);
}

float PapyrusInterface::SetArousalMultiplier(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	value = std::clamp(value, 0.0f, 100.f);

	PersistedData::ArousalMultiplierData::GetSingleton()->SetData(actorRef->formID, value);
	return value;
}

float PapyrusInterface::ModifyArousalMultiplier(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	float curMult = PersistedData::ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);
	float newVal = curMult + value;
	PersistedData::ArousalMultiplierData::GetSingleton()->SetData(actorRef->formID, newVal);
	return newVal;
}

float PapyrusInterface::GetArousalBaseline(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return LibidoManager::GetSingleton()->GetBaselineArousal(actorRef);
}

float PapyrusInterface::GetLibido(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return LibidoManager::GetSingleton()->GetBaseLibido(actorRef);
}

float PapyrusInterface::SetLibido(RE::StaticFunctionTag*, RE::Actor* actorRef, float newVal)
{
	return LibidoManager::GetSingleton()->SetBaseLibido(actorRef, newVal);
}

float PapyrusInterface::GetDaysSinceLastOrgasm(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	float lastOrgasmTime = PersistedData::LastOrgasmTimeData::GetSingleton()->GetData(actorRef->formID, 0.f);
	if (lastOrgasmTime < 0) {
		lastOrgasmTime = 0;
	}

	return RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastOrgasmTime;
}

bool PapyrusInterface::IsNaked(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return Utilities::Actor::IsNakedCached(actorRef);
}

bool PapyrusInterface::IsViewingNaked(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return Utilities::Actor::IsViewingNaked(actorRef);
}

bool PapyrusInterface::IsInScene(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return Utilities::Actor::IsParticipatingInScene(actorRef);
}

bool PapyrusInterface::IsViewingScene(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return Utilities::Actor::IsViewingScene(actorRef);
}

bool PapyrusInterface::IsWearingEroticArmor(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	if (!Utilities::Actor::IsNakedCached(actorRef)) {
		if (const auto eroticKeyword = Settings::GetSingleton()->GetEroticArmorKeyword()) {
			const auto wornKeywords = Utilities::Actor::GetWornArmorKeywords(actorRef);
			return wornKeywords.contains(eroticKeyword->formID);
		}
	}
	return false;
}

float PapyrusInterface::WornDeviceBaselineGain(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return DevicesIntegration::GetSingleton()->GetArousalBaselineFromDevices(actorRef);
}

bool PapyrusInterface::RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
{
	vm->RegisterFunction("GetArousal", "OSLArousedNative", GetArousal);
	vm->RegisterFunction("GetArousalMultiple", "OSLArousedNative", GetArousalMultiple);
	vm->RegisterFunction("GetArousalNoSideEffects", "OSLArousedNative", GetArousalNoSideEffects);

	vm->RegisterFunction("SetArousal", "OSLArousedNative", SetArousal);
	vm->RegisterFunction("SetArousalMultiple", "OSLArousedNative", SetArousalMultiple);
	vm->RegisterFunction("ModifyArousal", "OSLArousedNative", ModifyArousal);
	vm->RegisterFunction("ModifyArousalMultiple", "OSLArousedNative", ModifyArousalMultiple);

	vm->RegisterFunction("SetArousalMultiplier", "OSLArousedNative", SetArousalMultiplier);
	vm->RegisterFunction("GetArousalMultiplier", "OSLArousedNative", GetArousalMultiplier);
	vm->RegisterFunction("ModifyArousalMultiplier", "OSLArousedNative", ModifyArousalMultiplier);

	vm->RegisterFunction("GetArousalBaseline", "OSLArousedNative", GetArousalBaseline);
	vm->RegisterFunction("GetLibido", "OSLArousedNative", GetLibido);
	vm->RegisterFunction("SetLibido", "OSLArousedNative", SetLibido);

	vm->RegisterFunction("GetDaysSinceLastOrgasm", "OSLArousedNative", GetDaysSinceLastOrgasm);

	//Explainer
	vm->RegisterFunction("IsNaked", "OSLArousedNative", IsNaked);
	vm->RegisterFunction("IsViewingNaked", "OSLArousedNative", IsViewingNaked);
	vm->RegisterFunction("IsInScene", "OSLArousedNative", IsInScene);
	vm->RegisterFunction("IsViewingScene", "OSLArousedNative", IsViewingScene);
	vm->RegisterFunction("IsWearingEroticArmor", "OSLArousedNative", IsWearingEroticArmor);
	vm->RegisterFunction("WornDeviceBaselineGain", "OSLArousedNative", WornDeviceBaselineGain);

	return true;
}
