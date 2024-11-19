////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server objects for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch_script.h"
#include "xrServer_Objects_ALife.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

#pragma optimize("s",on)
void CSE_ALifeSchedulable::script_register(lua_State *L)
{
	module(L)[
		class_<IPureSchedulableObject>
			("ipure_schedulable_object"),
//			.def(		constructor<>()),
		
		class_<CSE_ALifeSchedulable,IPureSchedulableObject>
			("cse_alife_schedulable")
//			.def(		constructor<LPCSTR>())
	];
}

SCRIPT_EXPORT1(CSE_ALifeSchedulable);

void CSE_ALifeGraphPoint::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_ALifeGraphPoint,
			"cse_alife_graph_point",
			CSE_Abstract
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeGraphPoint, CSE_Abstract);

void CSE_ALifeObject::script_register(lua_State *L)
{
	module(L)[
		luabind_class_alife1(
			CSE_ALifeObject,
			"cse_alife_object",
			CSE_Abstract
		)
		.def_readwrite	("online",				&CSE_ALifeObject::m_bOnline)
		.def			("move_offline",		(bool (CSE_ALifeObject::*)() const)(&CSE_ALifeObject::move_offline))
		.def			("move_offline",		(void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::move_offline))
		.def			("visible_for_map",		(bool (CSE_ALifeObject::*)() const)(&CSE_ALifeObject::visible_for_map))
		.def			("visible_for_map",		(void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::visible_for_map))
		.def			("can_switch_online",	(void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::can_switch_online))
		.def			("can_switch_offline",	(void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::can_switch_offline))
		.def			("use_ai_locations",	(void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::use_ai_locations))
		.def_readwrite	("m_level_vertex_id",	&CSE_ALifeObject::m_tNodeID)
		.def_readwrite	("m_game_vertex_id",	&CSE_ALifeObject::m_tGraphID)
		.def_readwrite	("m_story_id",			&CSE_ALifeObject::m_story_id)
	];
}

SCRIPT_EXPORT2(CSE_ALifeObject, CSE_Abstract);

void CSE_ALifeGroupAbstract::script_register(lua_State *L)
{
	module(L)[
		class_<CSE_ALifeGroupAbstract>
			("cse_alife_group_abstract")
//			.def(		constructor<LPCSTR>())
	];
}

SCRIPT_EXPORT1(CSE_ALifeGroupAbstract);

void CSE_ALifeDynamicObject::script_register(lua_State *L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeDynamicObject,
			"cse_alife_dynamic_object",
			CSE_ALifeObject
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeDynamicObject, CSE_ALifeObject);

void CSE_ALifeDynamicObjectVisual::script_register(lua_State *L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeDynamicObjectVisual,
			"cse_alife_dynamic_object_visual",
			CSE_ALifeDynamicObject,
			CSE_Visual
		)
	];
}

SCRIPT_EXPORT3(CSE_ALifeDynamicObjectVisual, CSE_ALifeDynamicObject, CSE_Visual);

void CSE_ALifePHSkeletonObject::script_register(lua_State *L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifePHSkeletonObject,
			"cse_alife_ph_skeleton_object",
			CSE_ALifeDynamicObjectVisual,
			CSE_PHSkeleton
		)
	];
}

SCRIPT_EXPORT3(CSE_ALifePHSkeletonObject, CSE_ALifeDynamicObjectVisual, CSE_PHSkeleton);

void CSE_ALifeSpaceRestrictor::script_register(lua_State *L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeSpaceRestrictor,
			"cse_alife_space_restrictor",
			CSE_ALifeDynamicObject,
			CSE_Shape
		)
	];
}

SCRIPT_EXPORT3(CSE_ALifeSpaceRestrictor, CSE_ALifeDynamicObject, CSE_Shape);

void CSE_ALifeLevelChanger::script_register(lua_State *L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeLevelChanger,
			"cse_alife_level_changer",
			CSE_ALifeSpaceRestrictor
		)
	];
}

SCRIPT_EXPORT2(CSE_ALifeLevelChanger, CSE_ALifeSpaceRestrictor);

void CSE_ALifeInventoryBox::script_register(lua_State *L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeInventoryBox,
			"cse_alife_inventory_box",
			CSE_ALifeDynamicObjectVisual
			)
	];
}

SCRIPT_EXPORT2(CSE_ALifeInventoryBox, CSE_ALifeDynamicObjectVisual);
