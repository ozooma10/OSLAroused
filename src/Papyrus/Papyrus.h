#pragma once

namespace Papyrus
{

	//Actor State
	bool IsActorNaked(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	void RegisterSceneStart(RE::StaticFunctionTag* base, bool bIsOstim, int sceneId, RE::reference_array<RE::Actor*> actorRefs);
	void RemoveScene(RE::StaticFunctionTag* base, bool bIsOstim, int sceneId);

	void RegisterActorOrgasm(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	//Keyword Management
	bool AddKeywordToForm(RE::StaticFunctionTag* base, RE::TESForm* form, RE::BGSKeyword* keyword);
	bool RemoveKeywordFromForm(RE::StaticFunctionTag* base, RE::TESForm* form, RE::BGSKeyword* keyword);

	bool FormHasKeywordString(RE::StaticFunctionTag* base, RE::TESForm* form, RE::BSFixedString keyword);

	//Utilities
	float GenerateRandomFloat(RE::StaticFunctionTag* base, float min, float max);
	float ClampFloat(RE::StaticFunctionTag* base, float val, float min, float max);

	//Debug Functions
	void DumpArousalData(RE::StaticFunctionTag* base);
	void ClearAllArousalData(RE::StaticFunctionTag* base);

	bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);

	namespace Events
	{
		void SendActorArousalUpdatedEvent(RE::Actor* actorRef, float newExposure);
		void SendActorLibidoUpdatedEvent(RE::Actor* actorRef, float newLibido);

		void SendActorNakedUpdatedEvent(RE::Actor* actorRef, bool newNaked);
	}

};
