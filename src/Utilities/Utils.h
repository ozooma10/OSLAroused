#pragma once

#include <random>
#include "Managers/SceneManager.h"
#include "Managers/ActorStateManager.h"

enum class FactionType
{
	sla_Arousal,
	sla_Arousal_Locked,
	sla_Exhibitionist,
	sla_Exposure,
	sla_TimeRate,
	sla_ExposureRate,
	sla_GenderPreference
};

namespace Utilities
{
    void logInvalidArgsVerbose(const char* fnName);

	inline float GenerateRandomFloat(float min, float max)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> dis(min, max);
		return dis(mt);
	}

	namespace Forms
	{
		RE::FormID ResolveFormId(uint32_t modIndex, RE::FormID rawFormId);
	}

	class Factions
	{
	public:
		static Factions* GetSingleton()
		{
			static Factions singleton;
			return &singleton;
		}
		void Initialize();


		void SetFactionRank(RE::Actor* actorRef, FactionType factionType, int rank);
		int GetFactionRank(RE::Actor* actorRef, FactionType factionType);
	private:

		RE::TESFaction* m_ArousalFaction;
		RE::TESFaction* m_ArousalLockedFaction;
		RE::TESFaction* m_ExhibitionistFaction;
		RE::TESFaction* m_ExposureFaction;
		RE::TESFaction* m_TimeRateFaction;
		RE::TESFaction* m_ExposureRateFaction;
		RE::TESFaction* m_GenderPreferenceFaction;
	};

	//Keyword logic based off powerof3's CommonLibSSE implementation
	namespace Keywords
	{
		bool AddKeyword(RE::TESForm* form, RE::BGSKeyword* keyword);

		bool RemoveKeyword(RE::TESForm* form, RE::BGSKeyword* keyword);

		void DistributeKeywords();
	}

	namespace Actor
	{
		inline bool IsNaked(RE::Actor* actorRef)
		{
			return actorRef->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) == nullptr;
		}

		inline bool IsNakedCached(RE::Actor* actorRef)
		{
			return ActorStateManager::GetSingleton()->GetActorNaked(actorRef);
		}

		inline bool IsViewingNaked(RE::Actor* actorRef)
		{
			return ActorStateManager::GetSingleton()->GetActorSpectatingNaked(actorRef);
		}

		inline bool IsParticipatingInScene(RE::Actor* actorRef)
		{
			return SceneManager::GetSingleton()->IsActorParticipating(actorRef->GetHandle());
		}

		inline bool IsViewingScene(RE::Actor* actorRef)
		{
			return SceneManager::GetSingleton()->IsActorViewing(actorRef->GetHandle());
		}

		std::vector<RE::TESForm*> GetWornArmor(RE::Actor* actorRef);

		std::set<RE::FormID> GetWornArmorKeywords(RE::Actor* actorRef, RE::TESForm* armorToIgnore = nullptr);
	}

	namespace World
	{
        void ForEachReferenceInRange(RE::TESObjectREFR* origin, float radius,
                                     std::function<RE::BSContainer::ForEachResult(RE::TESObjectREFR& ref)> callback);
	}
}
#pragma once
