#pragma once

#include "Utilities/LRUCache.h"

float CalculateActorLibidoModifier(RE::Actor* actorFormId);

class LibidoManager
{
public:
	static LibidoManager* GetSingleton()
	{
		static LibidoManager singleton;
		return &singleton;
	}

	LibidoManager() :
		m_LibidoModifierCache(std::function<float(RE::Actor*)>(CalculateActorLibidoModifier), 100) {}
	
	float UpdateActorLibido(RE::Actor* actorRef, float gameHoursPassed, float targetLibido);

	float GetBaselineArousal(RE::Actor* actorRef);

	float GetBaseLibido(RE::Actor* actorRef);
	float SetBaseLibido(RE::Actor* actorRef, float newVal);
	float ModifyBaseLibido(RE::Actor* actorRef, float modVal);

	void ActorLibidoModifiersUpdated(RE::Actor* actorRef);

private:
	
	//Cache of additional Arousal values on top of actors base libido (Resulting in Baseline Arousal)
	Utilities::LRUCache<RE::Actor*, float> m_LibidoModifierCache;
};
