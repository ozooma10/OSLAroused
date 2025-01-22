#pragma once
#include "PCH.h"

using Lock = std::recursive_mutex;
using Locker = std::lock_guard<Lock>;

namespace RE {
	bool operator<(const ActorHandle& lhs, const ActorHandle& rhs);
}

class SceneManager
{
public:
	enum class SceneFramework
	{
		kSexLab,
		kOStim
	};

	struct SceneData
	{
		SceneFramework Framework;
		int SceneId;
		std::vector<RE::ActorHandle> Participants;
	};

	static SceneManager* GetSingleton()
	{
		static SceneManager singleton;
		return &singleton;
	}

	void RegisterScene(SceneData scene);
	void RemoveScene(SceneFramework framework, int sceneId);
	void ClearScenes();

	bool IsActorParticipating(RE::ActorHandle actorRef);
	bool IsActorViewing(RE::ActorHandle actorRef);

	//Updates timestamps for spectators in sceneviewingmap. 
	void UpdateSceneSpectators(std::set<RE::ActorHandle> spectators);

	std::vector<SceneData> GetAllScenes() const;

private:
	std::vector<SceneData> m_Scenes;

	std::map<RE::ActorHandle, bool> m_SceneParticipantMap;

	//ActorId, GameTime last in presence of scene
	std::map<RE::ActorHandle, float> m_SceneViewingMap;

	mutable Lock m_Lock;
};
