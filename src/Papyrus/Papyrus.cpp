#include "Papyrus.h"
#include "PersistedData.h"
#include "RuntimeEvents.h"
#include "Settings.h"
#include "Debug.h"
#include "Utilities/Utils.h"
#include "Managers/ActorStateManager.h"
#include "Managers/ArousalManager.h"
#include "Managers/SceneManager.h"



bool Papyrus::IsActorNaked(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return ActorStateManager::GetSingleton()->GetActorNaked(actorRef);
}

void Papyrus::RegisterSceneStart(RE::StaticFunctionTag*, bool bIsOstim, int sceneId, RE::reference_array<RE::Actor*> actorRefs)
{
	SceneManager::SceneData sceneData {
		bIsOstim ? SceneManager::SceneFramework::kOStim : SceneManager::SceneFramework::kSexLab,
		sceneId,
		actorRefs
	};
	SceneManager::GetSingleton()->RegisterScene(sceneData);
}

void Papyrus::RemoveScene(RE::StaticFunctionTag*, bool bIsOstim, int sceneId)
{
	SceneManager::GetSingleton()->RemoveScene(bIsOstim ? SceneManager::SceneFramework::kOStim : SceneManager::SceneFramework::kSexLab, sceneId);
}

void Papyrus::RegisterActorOrgasm(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	PersistedData::LastOrgasmTimeData::GetSingleton()->SetData(actorRef->formID, RE::Calendar::GetSingleton()->GetCurrentGameTime());
}

bool Papyrus::AddKeywordToForm(RE::StaticFunctionTag*, RE::TESForm* form, RE::BGSKeyword* keyword)
{
	if (!form || !keyword) {
		return false;
	}
	
	return Utilities::Keywords::AddKeyword(form, keyword);
}

bool Papyrus::RemoveKeywordFromForm(RE::StaticFunctionTag*, RE::TESForm* form, RE::BGSKeyword* keyword)
{
	if (!form || !keyword) {
		return false;
	}

	return Utilities::Keywords::RemoveKeyword(form, keyword);
}

bool Papyrus::FormHasKeywordString(RE::StaticFunctionTag*, RE::TESForm* form, RE::BSFixedString keyword)
{
	logger::error("FormHasKeywordString: {}.", keyword);
	if (!form) {
		logger::error("FormHasKeywordString received none obj.");
		return false;
	}
	RE::BGSKeywordForm* keywords = form->As<RE::BGSKeywordForm>();
	if (!keywords) {
		logger::error("Keywords cast failed.");
		return false;
	}
	const char* p1 = keyword.data();
	for (uint32_t i = 0; i < keywords->numKeywords; ++i) {
		const char* p2 = keywords->keywords[i]->formEditorID.data();
		logger::error("Keyword EditorId: {}.", p2);
		if (strstr(p2, p1) != NULL)
			return true;
	}
	return false;
}

float Papyrus::GenerateRandomFloat(RE::StaticFunctionTag*, float min, float max)
{
	return Utilities::GenerateRandomFloat(min, max);
}

float Papyrus::ClampFloat(RE::StaticFunctionTag*, float val, float min, float max)
{
	return val > max ? max : (val < min ? min : val);
}

void Papyrus::DumpArousalData(RE::StaticFunctionTag*)
{
	Debug::DumpAllArousalData();
}

void Papyrus::ClearAllArousalData(RE::StaticFunctionTag*)
{
	Debug::ClearAllArousalData();
}

bool Papyrus::RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
{
	//General State

	
	//Actor State
	vm->RegisterFunction("IsActorNaked", "OSLArousedNative", IsActorNaked);

	vm->RegisterFunction("RegisterSceneStart", "OSLArousedNative", RegisterSceneStart);
	vm->RegisterFunction("RemoveScene", "OSLArousedNative", RemoveScene);

	vm->RegisterFunction("RegisterActorOrgasm", "OSLArousedNative", RegisterActorOrgasm);

	//Keyword
	vm->RegisterFunction("AddKeywordToForm", "OSLArousedNative", AddKeywordToForm);
	vm->RegisterFunction("RemoveKeywordFromForm", "OSLArousedNative", RemoveKeywordFromForm);
	vm->RegisterFunction("FormHasKeywordString", "OSLArousedNative", FormHasKeywordString);
	
	//Utilities
	vm->RegisterFunction("GenerateRandomFloat", "OSLArousedNative", GenerateRandomFloat);
	
	//Debug
	vm->RegisterFunction("DumpArousalData", "OSLArousedNative", DumpArousalData);
	vm->RegisterFunction("ClearAllArousalData", "OSLArousedNative", ClearAllArousalData);

	return true;
}

void SendModEvent(RE::BSFixedString eventName, float numArg, RE::TESForm* sender)
{
	SKSE::ModCallbackEvent modEvent{
		eventName,
		RE::BSFixedString(),
		numArg,
		sender
	};

	auto modCallback = SKSE::GetModCallbackEventSource();
	modCallback->SendEvent(&modEvent);
}

void Papyrus::Events::SendActorArousalUpdatedEvent(RE::Actor* actorRef, float newExposure)
{
	SendModEvent("OSLA_ActorArousalUpdated", newExposure, actorRef);
}

void Papyrus::Events::SendActorLibidoUpdatedEvent(RE::Actor* actorRef, float newLibido)
{
	SendModEvent("OSLA_ActorLibidoUpdated", newLibido, actorRef);
}

void Papyrus::Events::SendActorNakedUpdatedEvent(RE::Actor* actorRef, bool newNaked)
{
	SendModEvent("OSLA_ActorNakedUpdated", newNaked ? 1.f : 0.f, actorRef);
}
