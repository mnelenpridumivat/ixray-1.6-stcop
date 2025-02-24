////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items_script2.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server items for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch_script.h"
#include "xrServer_Objects_ALife_Items.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

#pragma optimize("s",on)
void CSE_ALifeItemPDA::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemPDA,
			"cse_alife_item_pda",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemPDA, CSE_ALifeItem);

void CSE_ALifeItemDocument::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemDocument,
			"cse_alife_item_document",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemDocument, CSE_ALifeItem);

void CSE_ALifeItemGrenade::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemGrenade,
			"cse_alife_item_grenade",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemGrenade, CSE_ALifeItem);

void CSE_ALifeItemExplosive::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemExplosive,
			"cse_alife_item_explosive",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemExplosive, CSE_ALifeItem);

void CSE_ALifeItemBolt::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemBolt,
			"cse_alife_item_bolt",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemBolt, CSE_ALifeItem);

void CSE_ALifeItemsNotSave::script_register(lua_State* L) {
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemsNotSave,
			"cse_alife_item_not_save",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemsNotSave, CSE_ALifeItem);

void CSE_ALifeItemCustomOutfit::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemCustomOutfit,
			"cse_alife_item_custom_outfit",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemCustomOutfit, CSE_ALifeItem);

void CSE_ALifeItemHelmet::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemHelmet,
			"cse_alife_item_helmet",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemHelmet, CSE_ALifeItem);

void CSE_ALifeItemWeaponMagazined::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemWeaponMagazined,
			"cse_alife_item_weapon_magazined",
			CSE_ALifeItemWeapon
			)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemWeaponMagazined, CSE_ALifeItemWeapon);

void CSE_ALifeItemFlamethrower::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemFlamethrower,
			"cse_alife_item_flamethrower",
			CSE_ALifeItemWeapon
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemFlamethrower, CSE_ALifeItemWeapon);
