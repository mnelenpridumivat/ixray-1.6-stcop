#pragma once

class IKinematicsAnimated;

class CCutsceneManager {
	IKinematicsAnimated* HudModelKinematicsAnimated = nullptr;
	IKinematics* HudModelKinematics = nullptr;
	IRenderVisual* HudModel = nullptr;

	CCutsceneManager(){}
public:
	CCutsceneManager(const CCutsceneManager& other) = delete;
	CCutsceneManager(CCutsceneManager&& other) = delete;
	CCutsceneManager& operator=(const CCutsceneManager& other) = delete;
	CCutsceneManager& operator=(CCutsceneManager&& other) = delete;

	static CCutsceneManager& GetInstance();
	static void PlayCutscene(LPCSTR section);

	void Update();
};