#include "stdafx.h"
#include "pch_script.h"
#include "MagnetZone.h"
#include "base_client_classes.h"

using namespace luabind;

#pragma optimize("s",on)
void CMagnetZone::script_register(lua_State* L)
{
	module(L)
		[
			class_<CMagnetZone, CGameObject>("CMagnetZone")
			.def(constructor<>())
		];
}

SCRIPT_EXPORT2(CMagnetZone, CObjectScript);
