#pragma once

namespace PapyrusInterface
{
	float GetArousal(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	float GetArousalNoSideEffects(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	std::vector<float> GetArousalMultiple(RE::StaticFunctionTag* base, RE::reference_array<RE::Actor*> actorRefs);

	float SetArousal(RE::StaticFunctionTag* base, RE::Actor* actorRef, float value);
	void SetArousalMultiple(RE::StaticFunctionTag* base, RE::reference_array<RE::Actor*> actorRefs, float value);

	float ModifyArousal(RE::StaticFunctionTag* base, RE::Actor* actorRef, float value);
	void ModifyArousalMultiple(RE::StaticFunctionTag* base, RE::reference_array<RE::Actor*> actorRefs, float value);

	float GetArousalMultiplier(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	float SetArousalMultiplier(RE::StaticFunctionTag* base, RE::Actor* actorRef, float value);
	float ModifyArousalMultiplier(RE::StaticFunctionTag* base, RE::Actor* actorRef, float value);

	float GetArousalBaseline(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	float GetLibido(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	float SetLibido(RE::StaticFunctionTag* base, RE::Actor* actorRef, float newVal);

	
	float GetDaysSinceLastOrgasm(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	bool IsNaked(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	bool IsViewingNaked(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	bool IsInScene(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	bool IsViewingScene(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	bool IsWearingEroticArmor(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	float WornDeviceBaselineGain(RE::StaticFunctionTag* base, RE::Actor* actorRef);


	bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);
}
