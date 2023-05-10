#include "Managers/SceneManager.h"
#include "Managers/LibidoManager.h"

void SceneManager::RegisterScene(SceneData scene)
{
	Locker locker(m_Lock);
	m_Scenes.push_back(scene);

	for(auto & partcipant : scene.Participants) {
		m_SceneParticipantMap[partcipant] = true;
		LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated(partcipant);
	}
}

void SceneManager::RemoveScene(SceneFramework framework, int sceneId)
{
	Locker locker(m_Lock);
	auto scenesToRemove = std::remove_if(m_Scenes.begin(), m_Scenes.end(), [framework, sceneId](const SceneManager::SceneData& scene) {
		return scene.Framework == framework && scene.SceneId == sceneId;
	});
	if (scenesToRemove != m_Scenes.end()) {
		for (auto it = scenesToRemove; it != m_Scenes.end(); it++) {
			for (auto& partcipant : (*it).Participants) {
				m_SceneParticipantMap[partcipant] = false;
				LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated(partcipant);
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
		//@TODO: Calculate time based off global update cycle [not just 0.72 game hours]
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
	for (auto itr = m_SceneViewingMap.begin(); itr != m_SceneViewingMap.end();) {
		if (!spectators.contains((*itr).first)) {
			LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated((*itr).first);
			itr = m_SceneViewingMap.erase(itr);
		} else {
			itr++;
		}
	}

	float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
	for (const auto spectator : spectators) {
		m_SceneViewingMap[spectator] = currentTime;
		LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated(spectator);
	}
}

std::vector<SceneManager::SceneData> SceneManager::GetAllScenes() const
{
	Locker locker(m_Lock);
	return m_Scenes;
}

