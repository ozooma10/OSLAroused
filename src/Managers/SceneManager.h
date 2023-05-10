#pragma once
#include "PCH.h"

using Lock = std::recursive_mutex;
using Locker = std::lock_guard<Lock>;

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
		std::vector<RE::Actor*> Participants;
	};

	static SceneManager* GetSingleton()
	{
		static SceneManager singleton;
		return &singleton;
	}

	void RegisterScene(SceneData scene);
	void RemoveScene(SceneFramework framework, int sceneId);
	void ClearScenes();

	bool IsActorParticipating(RE::Actor* actorRef);
	bool IsActorViewing(RE::Actor* actorRef);

	//Updates timestamps for spectators in sceneviewingmap. 
	void UpdateSceneSpectators(std::set<RE::Actor*> spectators);

	std::vector<SceneData> GetAllScenes() const;

private:
	std::vector<SceneData> m_Scenes;

	std::map<RE::Actor*, bool> m_SceneParticipantMap;
	//ActorId, GameTime last in presence of scene
	std::map<RE::Actor*, float> m_SceneViewingMap;

	mutable Lock m_Lock;
};
