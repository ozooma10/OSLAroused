#pragma once
#include "PCH.h"
#include "Utilities/LRUCache.h"

bool IsActorNaked(RE::Actor* actorRef);

const static RE::FormID kActorTypeCreatureKeywordFormId = 0x13795;
const static RE::FormID kActorTypeAnimalKeywordFormId = 0x13798;

class ActorStateManager
{
public:

	static ActorStateManager* GetSingleton()
	{
		static ActorStateManager singleton;
		return &singleton;
	}

	ActorStateManager() :
		m_ActorNakedStateCache(std::function<bool(RE::Actor*)>(IsActorNaked), 100),
		m_CreatureKeyword((RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId)),
		m_AnimalKeyword((RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId)) {}

	static bool GetActorArousalLocked(RE::Actor* actorRef);
	static void SetActorArousalLocked(RE::Actor* actorRef, bool locked);

	bool GetActorNaked(RE::Actor* actorRef);
	void ActorNakedStateChanged(RE::Actor* actorRef, bool newNaked);

	// Drop all cached naked states so they re-evaluate on next fetch. Used when a setting
	// that feeds IsNaked changes globally (e.g. an armor's "counts as clothing" flag).
	void ClearAllNakedStates();

	bool GetActorSpectatingNaked(RE::Actor* actorRef);
	float GetSpectatingMaxNudityScore(RE::Actor* actorRef);
	void UpdateActorsSpectating(std::map<RE::Actor*, float> spectatorNudityScores);

	//Returns true if actor is non-creature, non-animal npc
	bool IsHumanoidActor(RE::Actor* actorRef);

	void OnActorArousalUpdated(RE::Actor* actorRef, float newArousal);

	// Drive the actors SOS bend animation directly from the current arousal value
	// deduped by bend bucket so an unchanged state emits notion.
	// NotifyAnimationGraph call is marshalled to the main thread internally, so this is safe to call from the Papyrus VM thread.
	void UpdateSOSAnimation(RE::Actor* actorRef, float arousal);

	// decides wheather to emit OSLA_ActorArousalUpdated for this actor given the new arousal value
	// comparies against the last notified value. The player always notifies.
	bool ShouldNotifyArousalChange(RE::Actor* actorRef, float newArousal);

	RE::Actor* GetMostArousedActorInLocation();

private:
	void HandlePlayerArousalUpdated(RE::Actor* actorRef, float newArousal);

private:

	struct SpectatingData {
		float maxNudityScore;
		float lastUpdateTime;
	};

	Utilities::LRUCache<RE::Actor*, bool> m_ActorNakedStateCache;

	std::map<RE::Actor*, SpectatingData> m_NakedSpectatingMap;
	// Guards m_NakedSpectatingMap: written on the main thread (UpdateActorsSpectating,
	// from the arousal tick) and read from the Papyrus VM thread via the libido path.
	std::mutex m_SpectatingLock;

	// [formid, sosbendbucket] cache of last sent actor SOS state
	// updated from both main and papyrusVM threads
	std::unordered_map<RE::FormID, int> m_SosStateCache;
	std::mutex m_SosStateLock;

	// last arousal value broadcast via OSLA_ActorArousalUpdated per actor
	std::unordered_map<RE::FormID, float> m_LastSentArousalCache;
	std::mutex m_LastSentArousalLock;

	RE::BGSKeyword* m_CreatureKeyword;
	RE::BGSKeyword* m_AnimalKeyword;

	//Player Variables
	float m_PreviousPlayerArousal = 0.f;
	float m_LastNotificationTime = 0.f;
	bool m_WasPlayerDispleased = false;

	//Actor Variables
	RE::ActorHandle m_MostArousedActor;
	float m_MostArousedActorArousal = 0.f;
};
