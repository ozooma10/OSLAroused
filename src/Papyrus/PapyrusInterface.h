#pragma once

namespace PapyrusInterface
{
	// =================== SLA/OSL Mode Shared Methods =======================
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

	float GetLibido(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	float SetLibido(RE::StaticFunctionTag* base, RE::Actor* actorRef, float newVal);
	float ModifyLibido(RE::StaticFunctionTag* base, RE::Actor* actorRef, float modVal);
	
	// =================== OSL Mode Methods =======================
	
	float GetArousalBaseline(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	// =================== SLA Mode Methods =======================
	float GetExposure(RE::StaticFunctionTag* base, RE::Actor* actorRef);


	// ================== = ACTOR STATE ====================== =

	float GetDaysSinceLastOrgasm(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	bool IsNaked(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	bool IsViewingNaked(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	bool IsInScene(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	bool IsViewingScene(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	bool IsWearingEroticArmor(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	// A.N.D. Integration debug function
	float GetANDNudityScore(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	bool IsActorExhibitionist(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	void SetActorExhibitionist(RE::StaticFunctionTag* base, RE::Actor* actorRef, bool exhibitionist);

	bool IsActorArousalLocked(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	void SetActorArousalLocked(RE::StaticFunctionTag* base, RE::Actor* actorRef, bool locked);

	float GetActorTimeRate(RE::StaticFunctionTag* base, RE::Actor* actorRef);
	float SetActorTimeRate(RE::StaticFunctionTag* base, RE::Actor* actorRef, float timeRate);
	float ModifyActorTimeRate(RE::StaticFunctionTag* base, RE::Actor* actorRef, float timeRate);

	float WornDeviceBaselineGain(RE::StaticFunctionTag* base, RE::Actor* actorRef);


	bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);

	std::vector<RE::Actor*> GetLastScannedActors(RE::StaticFunctionTag* base);

	RE::Actor* GetMostArousedActorInLocation(RE::StaticFunctionTag* base);
}
