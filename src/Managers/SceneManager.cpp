#include "Managers/SceneManager.h"
#include "Managers/ArousalManager.h"
#include "Managers/ArousalSystem/ArousalSystemOSL.h"


void SceneManager::RegisterScene(SceneData scene)
{
	Locker locker(m_Lock);
	m_Scenes.push_back(scene);
	auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem());
	for(auto& participant: scene.Participants) {
		if (auto actor = participant.get()) {
			m_SceneParticipantMap[participant] = true;

			//Only emit update events for OSL mode
			if (oslSystem) {
				oslSystem->ActorLibidoModifiersUpdated(actor.get());
			}
		}
	}
}

void SceneManager::RemoveScene(SceneFramework framework, int sceneId)
{
	Locker locker(m_Lock);
	auto scenesToRemove = std::remove_if(m_Scenes.begin(), m_Scenes.end(), [framework, sceneId](const SceneManager::SceneData& scene) {
		return scene.Framework == framework && scene.SceneId == sceneId;
	});
	if (scenesToRemove != m_Scenes.end()) {
		auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem());
		for (auto it = scenesToRemove; it != m_Scenes.end(); it++) {
			for (auto& participant : (*it).Participants) {
				if (auto actor = participant.get()) {
					m_SceneParticipantMap[participant] = false;
					//Only emit update events for OSL mode
					if (oslSystem) {
						oslSystem->ActorLibidoModifiersUpdated(actor.get());
					}
				}
			}
		}
		
		m_Scenes.erase(
			scenesToRemove,
			m_Scenes.end()
		);

		//If there are no more scenes, than clear the maps
		if (m_Scenes.empty()) {
			m_SceneParticipantMap.clear();

			//Update libido modifiers for all m_sceneviewingmap actors
			for (auto& actor : m_SceneViewingMap) {
				if (auto actorPtr = actor.first.get()) {
					oslSystem->ActorLibidoModifiersUpdated(actorPtr.get());
				}
			}
			m_SceneViewingMap.clear();
		}
	}
}

void SceneManager::ClearScenes()
{
	Locker locker(m_Lock);
	m_Scenes.clear();
	m_SceneParticipantMap.clear();
	m_SceneViewingMap.clear();
}

bool SceneManager::IsActorParticipating(RE::ActorHandle actorRef)
{
	Locker locker(m_Lock);
	auto it = m_SceneParticipantMap.find(actorRef);
	return it != m_SceneParticipantMap.end() && it->second;
}

bool SceneManager::IsActorViewing(RE::ActorHandle actorRef)
{
	Locker locker(m_Lock);
	auto it = m_SceneViewingMap.find(actorRef);
	if (it != m_SceneViewingMap.end()) {
		//TODO: Calculate time based off global update cycle [not just 0.72 game hours]
		if (RE::Calendar::GetSingleton()->GetCurrentGameTime() - it->second < 0.1f) {
			return true;
		}
	}
	return false;
}

void SceneManager::UpdateSceneSpectators(std::set<RE::ActorHandle> spectators)
{
	Locker locker(m_Lock);
	logger::trace("Updating Scene Spectators {}", spectators.size());

	//Remove any old spectators from map who are not in spectators set
	//Need to do this to purge libido modifier cache
	auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem());
	for (auto itr = m_SceneViewingMap.begin(); itr != m_SceneViewingMap.end();) {
		if (!spectators.contains((*itr).first)) {
			//Only emit update events for OSL mode
			if (auto actor = (*itr).first.get(); oslSystem && actor) {
				oslSystem->ActorLibidoModifiersUpdated(actor.get());
			}
			itr = m_SceneViewingMap.erase(itr);
		} else {
			itr++;
		}
	}

	float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
	for (const auto spectator : spectators) {
		if (auto actor = spectator.get()) {
			m_SceneViewingMap[spectator] = currentTime;
			//Only emit update events for OSL mode
			if (oslSystem) {
				oslSystem->ActorLibidoModifiersUpdated(actor.get());
			}
		}
	}
}

std::vector<SceneManager::SceneData> SceneManager::GetAllScenes() const
{
	Locker locker(m_Lock);
	return m_Scenes;
}

bool RE::operator<(const ActorHandle& lhs, const ActorHandle& rhs)
{
	return lhs.native_handle() < rhs.native_handle();
}
