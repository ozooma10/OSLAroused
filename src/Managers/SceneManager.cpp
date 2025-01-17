#include "Managers/SceneManager.h"
#include "Managers/ArousalManager.h"

void SceneManager::RegisterScene(SceneData scene)
{
	Locker locker(m_Lock);
	m_Scenes.push_back(scene);
	auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem());
	for(auto & partcipant : scene.Participants) {
		m_SceneParticipantMap[partcipant] = true;

		//Only emit update events for OSL mode
		if (oslSystem) {
			oslSystem->ActorLibidoModifiersUpdated(partcipant);
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
			for (auto& partcipant : (*it).Participants) {
				m_SceneParticipantMap[partcipant] = false;
				//Only emit update events for OSL mode
				if (oslSystem) {
					oslSystem->ActorLibidoModifiersUpdated(partcipant);
				}
			}
		}
		
		m_Scenes.erase(
			scenesToRemove
		);
	}
	
}

void SceneManager::ClearScenes()
{
	Locker locker(m_Lock);
	m_Scenes.clear();
}

bool SceneManager::IsActorParticipating(RE::Actor* actorRef)
{
	return m_SceneParticipantMap[actorRef];
}

bool SceneManager::IsActorViewing(RE::Actor* actorRef)
{
	if (const auto lastViewedGameTime = m_SceneViewingMap[actorRef]) {
		//TODO: Calculate time based off global update cycle [not just 0.72 game hours]
		if (RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastViewedGameTime < 0.1f) {
			return true;
		}
	}
	return false;
}

void SceneManager::UpdateSceneSpectators(std::set<RE::Actor*> spectators)
{
	//Remove any old spectators from map who are not in spectators set
	//Need to do this to purge libido modifier cache
	auto* oslSystem = dynamic_cast<ArousalSystemOSL*>(&ArousalManager::GetSingleton()->GetArousalSystem());
	for (auto itr = m_SceneViewingMap.begin(); itr != m_SceneViewingMap.end();) {
		if (!spectators.contains((*itr).first)) {
			//Only emit update events for OSL mode
			if (oslSystem) {
				oslSystem->ActorLibidoModifiersUpdated((*itr).first);
			}
			itr = m_SceneViewingMap.erase(itr);
		} else {
			itr++;
		}
	}

	float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
	for (const auto spectator : spectators) {
		m_SceneViewingMap[spectator] = currentTime;
		//Only emit update events for OSL mode
		if (oslSystem) {
			oslSystem->ActorLibidoModifiersUpdated(spectator);
		}
	}
}

std::vector<SceneManager::SceneData> SceneManager::GetAllScenes() const
{
	Locker locker(m_Lock);
	return m_Scenes;
}

