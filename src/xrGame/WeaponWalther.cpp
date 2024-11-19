#include "StdAfx.h"
#include "pch_script.h"
#include "WeaponWalther.h"

CWeaponWalther::CWeaponWalther(void)
{}

CWeaponWalther::~CWeaponWalther(void)
{}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponWalther::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponWalther,CGameObject>("CWeaponWalther")
			.def(constructor<>())
	];
}

SCRIPT_EXPORT2(CWeaponWalther, CObjectScript);
