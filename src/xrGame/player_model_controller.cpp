#include "stdafx.h"
#include "player_model_controller.h"
#include <player_hud.h>
#include <player_model.h>

CPlayerModelController& CPlayerModelController::GetInstance()
{
	// TODO: insert return statement here
	static CPlayerModelController controller;
	return controller;
}

void CPlayerModelController::UpdateControllerMode()
{
	// TODO: implement switch of using hud or full model
}

void CPlayerModelController::DestroyModel()
{
	if (PlayerHudModel) {
		PlayerHudModel = nullptr;
	}
}

void CPlayerModelController::CreateModel()
{
	if (IsUseFullBodyModel_Internal()) {
		PlayerHudModel.reset(new player_model());
	}
	else {
		PlayerHudModel.reset(new player_hud());
	}
	PlayerHudModel->load_default();
}

bool CPlayerModelController::IsHudValid()
{
	return PlayerHudModel.get();
}

first_person_player_interface* CPlayerModelController::GetHud()
{
	R_ASSERT(PlayerHudModel);
	return PlayerHudModel.get();
}

CPlayerModelController::CPlayerModelController() {

}

bool CPlayerModelController::IsUseFullBodyModel_Internal()
{
	VERIFY(pSettings);
	bUseFullBodyModel = pSettings->r_bool("actor_model_settings", "use_full_body_model");
	return bUseFullBodyModel;
}

bool CPlayerModelController::IsUseFullBodyModel()
{
	return bUseFullBodyModel;
}
