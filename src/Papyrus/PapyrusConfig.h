#pragma once

namespace PapyrusConfig
{
	//OSLAroused Settings
	void SetMinLibidoValue(RE::StaticFunctionTag* base, bool bPlayerVal, float newVal);
	void SetArousalChangeRate(RE::StaticFunctionTag* base, float newVal);
	void SetLibidoChangeRate(RE::StaticFunctionTag* base, float newVal);

	void SetSceneParticipantBaseline(RE::StaticFunctionTag* base, float newVal);
	void SetSceneViewingBaseline(RE::StaticFunctionTag* base, float newVal);
	void SetSceneVictimGainsArousal(RE::StaticFunctionTag* base, bool newVal);
	void SetBeingNudeBaseline(RE::StaticFunctionTag* base, float newVal);
	void SetViewingNudeBaseline(RE::StaticFunctionTag* base, float newVal);

	void SetEroticArmorBaseline(RE::StaticFunctionTag* base, float newVal, RE::BGSKeyword* keyword);

	void SetDeviceTypesBaseline1(RE::StaticFunctionTag* base, float belt, float collar, float legCuffs, float armCuffs, float bra, float gag, float p1, float p2, float blindfold, float harness);
	void SetDeviceTypesBaseline2(RE::StaticFunctionTag* base, float plugVag, float plugAnal, float corset, float boots, float gloves, float hood, float suit, float heavyBondage, float bondageMittens);
	void SetDeviceTypeBaseline(RE::StaticFunctionTag* base, int deviceTypeId, float newVal);

	bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);
}
