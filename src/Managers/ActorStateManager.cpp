#include "ActorStateManager.h"
#include "Utilities/Utils.h"
#include "Papyrus/Papyrus.h"
#include "Managers/ArousalManager.h"

bool IsActorNaked(RE::Actor* actorRef)
{
	return Utilities::Actor::IsNaked(actorRef);
}

bool ActorStateManager::GetActorNaked(RE::Actor* actorRef)
{
	return m_ActorNakedStateCache(actorRef);
}

void ActorStateManager::ActorNakedStateChanged(RE::Actor* actorRef, bool newNaked)
{
	m_ActorNakedStateCache.UpdateItem(actorRef, newNaked);
	Papyrus::Events::SendActorNakedUpdatedEvent(actorRef, newNaked);
	
	//Actor Naked updated so remove libido cache entry to force refresh on next fetch
			//Only emit update events for OSL mode
	if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
		oslSystem->ActorLibidoModifiersUpdated(actorRef);
	}
}

bool ActorStateManager::GetActorSpectatingNaked(RE::Actor* actorRef)
{
	if (const auto lastViewedGameTime = m_NakedSpectatingMap[actorRef]) {
		if (RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastViewedGameTime < 0.1f) {
			return true;
		}
	}
	return false;
}

void ActorStateManager::UpdateActorsSpectating(std::set<RE::Actor*> spectators)
{
	//Remove any old spectators from map who are not in spectators set
	//Need to do this to purge libido modifier cache
	for (auto itr = m_NakedSpectatingMap.begin(); itr != m_NakedSpectatingMap.end();) {
		if (!spectators.contains((*itr).first)) {
			if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
				oslSystem->ActorLibidoModifiersUpdated((*itr).first);
			}
			itr = m_NakedSpectatingMap.erase(itr);
		} else {
			itr++;
		}
	}

	float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
	for (const auto spectator : spectators) {
		m_NakedSpectatingMap[spectator] = currentTime;
		if (auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem())) {
			oslSystem->ActorLibidoModifiersUpdated(spectator);
		}
	}
}

bool ActorStateManager::IsHumanoidActor(RE::Actor* actorRef)
{
	if (!m_CreatureKeyword) {
		m_CreatureKeyword = (RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId);
	}
	if (!m_AnimalKeyword) {
		m_AnimalKeyword = (RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId);
	}

	if (m_CreatureKeyword && m_AnimalKeyword) {
		return !actorRef->HasKeyword(m_CreatureKeyword) && !actorRef->HasKeyword(m_AnimalKeyword);
	}

	return false;
}
