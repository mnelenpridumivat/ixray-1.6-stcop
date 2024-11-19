#include "StdAfx.h"
#include "pch_script.h"
#include "WeaponGroza.h"

CWeaponGroza::CWeaponGroza() :CWeaponMagazinedWGrenade(SOUND_TYPE_WEAPON_SUBMACHINEGUN) 
{}

CWeaponGroza::~CWeaponGroza() 
{}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponGroza::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponGroza,CGameObject>("CWeaponGroza")
			.def(constructor<>())
	];
}

SCRIPT_EXPORT2(CWeaponGroza, CObjectScript);
