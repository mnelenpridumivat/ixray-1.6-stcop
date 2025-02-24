////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server items for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch_script.h"
#include "xrServer_Objects_ALife_Items.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

#pragma optimize("s",on)
void CSE_ALifeInventoryItem::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeInventoryItem>
			("cse_alife_inventory_item")
//			.def(		constructor<LPCSTR>())
	];
}

SCRIPT_EXPORT1(CSE_ALifeInventoryItem);

void CSE_ALifeItem::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item2(
//		luabind_class_abstract2(
			CSE_ALifeItem,
			"cse_alife_item",
			CSE_ALifeDynamicObjectVisual,
			CSE_ALifeInventoryItem
		)
	];
}

SCRIPT_EXPORT3(CSE_ALifeItem, CSE_ALifeDynamicObjectVisual, CSE_ALifeInventoryItem);

void CSE_ALifeItemTorch::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemTorch,
			"cse_alife_item_torch",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemTorch, CSE_ALifeItem);

void CSE_ALifeItemAmmo::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemAmmo,
			"cse_alife_item_ammo",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemAmmo, CSE_ALifeItem);

void CSE_ALifeItemFuel::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemFuel,
			"cse_alife_item_fuel",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemFuel, CSE_ALifeItem);

void CSE_ALifeItemWeapon::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemWeapon,
			"cse_alife_item_weapon",
			CSE_ALifeItem
		)
		.enum_("addon_flag")
		[
			value("eWeaponAddonGrenadeLauncher", static_cast<int>(CSE_ALifeItemWeapon::EWeaponAddonState::eWeaponAddonGrenadeLauncher)),
			value("eWeaponAddonScope", static_cast<int>(CSE_ALifeItemWeapon::EWeaponAddonState::eWeaponAddonScope)),
			value("eWeaponAddonSilencer", static_cast<int>(CSE_ALifeItemWeapon::EWeaponAddonState::eWeaponAddonSilencer)),
			//value("eWeaponAddonLaserDesignator", static_cast<int>(CSE_ALifeItemWeapon::EWeaponAddonState::eWeaponAddonLaserDesignator)),
			//value("eWeaponAddonTacticalTorch", static_cast<int>(CSE_ALifeItemWeapon::EWeaponAddonState::eWeaponAddonTacticalTorch)),
			value("eAddonAttachable", static_cast<int>(CSE_ALifeItemWeapon::EWeaponAddonStatus::eAddonAttachable)),
			value("eAddonDisabled", static_cast<int>(CSE_ALifeItemWeapon::EWeaponAddonStatus::eAddonDisabled)),
			value("eAddonPermanent", static_cast<int>(CSE_ALifeItemWeapon::EWeaponAddonStatus::eAddonPermanent))
		]
		.def("clone_addons",						&CSE_ALifeItemWeapon::clone_addons)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemWeapon, CSE_ALifeItem);

void CSE_ALifeItemWeaponShotGun::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemWeaponShotGun,
			"cse_alife_item_weapon_shotgun",
			CSE_ALifeItemWeapon
			)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemWeaponShotGun, CSE_ALifeItemWeapon);

void CSE_ALifeItemWeaponAutoShotGun::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemWeaponAutoShotGun,
			"cse_alife_item_weapon_auto_shotgun",
			CSE_ALifeItemWeapon
			)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemWeaponAutoShotGun, CSE_ALifeItemWeapon);

void CSE_ALifeItemDetector::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemDetector,
			"cse_alife_item_detector",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemDetector, CSE_ALifeItem);

void CSE_ALifeItemArtefact::script_register(lua_State *L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemArtefact,
			"cse_alife_item_artefact",
			CSE_ALifeItem
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeItemArtefact, CSE_ALifeItem);
