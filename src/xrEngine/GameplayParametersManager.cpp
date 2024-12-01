#include "stdafx.h"
#include "GameplayParametersManager.h"

GameplayParametersManager& GameplayParametersManager::GetInstance()
{
	static GameplayParametersManager manager;
	return manager;
}
