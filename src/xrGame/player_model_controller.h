#pragma once
#include "first_person_player_interface.h"

class player_hud;

class CPlayerModelController {

	bool bUseFullBodyModel = false;
	xr_unique_ptr<first_person_player_interface> PlayerHudModel = nullptr;

	CPlayerModelController();

	bool IsUseFullBodyModel_Internal();
public:

	bool IsUseFullBodyModel();

	CPlayerModelController(const CPlayerModelController& other) = delete;
	CPlayerModelController(CPlayerModelController&& other) = delete;
	CPlayerModelController& operator=(const CPlayerModelController& other) = delete;
	CPlayerModelController& operator=(CPlayerModelController&& other) = delete;

	static CPlayerModelController& GetInstance();

	void UpdateControllerMode();

	void DestroyModel();
	void CreateModel();

	bool IsHudValid();
	first_person_player_interface* GetHud();
};