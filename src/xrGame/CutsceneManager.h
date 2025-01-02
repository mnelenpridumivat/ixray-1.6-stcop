#pragma once

class IKinematicsAnimated;

class CCutsceneManager {
	IKinematicsAnimated* HudModelKinematicsAnimated = nullptr;
	IKinematics* HudModelKinematics = nullptr;
	IRenderVisual* HudModel = nullptr;
	Fvector Deviation;

#ifndef MASTER_GOLD
	bool Adjust = false;
	shared_str AdjustCutsceneSection;
	Fvector AdjustDeviation;
#endif

	CCutsceneManager(){}
public:
	CCutsceneManager(const CCutsceneManager& other) = delete;
	CCutsceneManager(CCutsceneManager&& other) = delete;
	CCutsceneManager& operator=(const CCutsceneManager& other) = delete;
	CCutsceneManager& operator=(CCutsceneManager&& other) = delete;

	static CCutsceneManager& GetInstance();
	static void PlayCutscene(LPCSTR section);

	void Update();

#ifndef MASTER_GOLD
	inline void SetAdjust(bool Adjust) { this->Adjust = Adjust; }
	inline bool GetAdjust() { return Adjust; }
	inline void SetAdjustSection(shared_str str) { AdjustCutsceneSection = str; }
	void SaveAdjust();
	void ResetAdjust();
	Fvector GetAdjustDelta();
#endif
};