#pragma once
#include "../xrEngine/AI/game_level_cross_table.h"

class CGameLevelCrossTable :public  IGameLevelCrossTable
{

public:
	CGameLevelCrossTable(IReader& reader, bool VerificationMode = false);
	
};