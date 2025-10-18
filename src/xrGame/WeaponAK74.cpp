#include "StdAfx.h"
#include "pch_script.h"
#include "WeaponAK74.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponAK74::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponAK74,CGameObject>("CWeaponAK74")
			.def(constructor<>())
	];
}

SCRIPT_EXPORT2(CWeaponAK74, CObjectScript);
