#include "Papyrus.h"
#include "PersistedData.h"
#include "RuntimeEvents.h"
#include "Settings.h"
#include "Debug.h"
#include "Utilities/Utils.h"
#include "Managers/ActorStateManager.h"
#include "Managers/ArousalManager.h"
#include "Managers/SceneManager.h"
#include "Config.h"


bool Papyrus::IsActorNaked(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return ActorStateManager::GetSingleton()->GetActorNaked(actorRef);
}

void Papyrus::RegisterSceneStart(RE::StaticFunctionTag*, bool bIsOstim, int sceneId, RE::reference_array<RE::Actor*> actorRefs)
{
	logger::trace("RegisterSceneStart: bIsOstim: {} sceneId: {} numACtors: {}.", bIsOstim, sceneId, actorRefs.size());

	SceneManager::SceneData sceneData{
		bIsOstim ? SceneManager::SceneFramework::kOStim : SceneManager::SceneFramework::kSexLab,
		sceneId,
		actorRefs
	};
	SceneManager::GetSingleton()->RegisterScene(sceneData);
}

void Papyrus::RemoveScene(RE::StaticFunctionTag*, bool bIsOstim, int sceneId)
{
	logger::trace("RemoveScene: bIsOstim: {} sceneId: {}.", bIsOstim, sceneId);
	SceneManager::GetSingleton()->RemoveScene(bIsOstim ? SceneManager::SceneFramework::kOStim : SceneManager::SceneFramework::kSexLab, sceneId);
}

void Papyrus::RegisterActorOrgasm(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	logger::trace("RegisterActorOrgasm: Actor: {}.", actorRef->GetDisplayFullName());
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

std::vector<RE::BSFixedString> Papyrus::GetRegisteredKeywords(RE::StaticFunctionTag* base)
{
	std::vector<RE::BSFixedString> registeredKeywords;
	for (auto& keyword : Config::GetSingleton()->GetRegisteredKeywords()) {
		registeredKeywords.emplace_back(keyword.EditorId);
	}

	return registeredKeywords;
}

bool Papyrus::RegisterNewKeyword(RE::StaticFunctionTag* base, RE::BSFixedString keywordEditorId)
{
	auto keywordForm = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(keywordEditorId);
	if (!keywordForm) {
		return false;
	}

	return Config::GetSingleton()->RegisterKeyword(keywordEditorId.c_str());
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
	vm->RegisterFunction("GetRegisteredKeywords", "OSLArousedNative", GetRegisteredKeywords);
	vm->RegisterFunction("RegisterNewKeyword", "OSLArousedNative", RegisterNewKeyword);

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
	SKSE::GetTaskInterface()->AddTask([actorRef, newExposure]() {
		SendModEvent("OSLA_ActorArousalUpdated", newExposure, actorRef);
	});
}

void Papyrus::Events::SendActorLibidoUpdatedEvent(RE::Actor* actorRef, float newLibido)
{
	SKSE::GetTaskInterface()->AddTask([actorRef, newLibido]() {
		SendModEvent("OSLA_ActorLibidoUpdated", newLibido, actorRef);
	});
}

void Papyrus::Events::SendActorNakedUpdatedEvent(RE::Actor* actorRef, bool newNaked)
{
	SKSE::GetTaskInterface()->AddTask([actorRef, newNaked]() {
		SendModEvent("OSLA_ActorNakedUpdated", newNaked ? 1.f : 0.f, actorRef);
	});
}

void Papyrus::Events::SendUpdateCompleteEvent(float numNearbyActors)
{
	SKSE::GetTaskInterface()->AddTask([numNearbyActors]() {
		//Clamp actor count to 20 (since sla limitation)
		SendModEvent("sla_UpdateComplete", std::clamp(numNearbyActors, 0.f, 20.f), nullptr);
	});
}
