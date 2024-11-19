////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes_script.cpp
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "pch_script.h"
#include "base_client_classes.h"
#include "base_client_classes_wrappers.h"
#include "../xrEngine/Feel_Sound.h"
//#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/Kinematics.h"
#include "ai/stalker/ai_stalker.h"
#include "patrol_point.h"
#include "patrol_path.h"

using namespace luabind;

#pragma optimize("s",on)
void DLL_PureScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<DLL_Pure, CDLL_PureWrapper>("DLL_Pure")
			.def(constructor<>())
			.def("_construct",&DLL_Pure::_construct,&CDLL_PureWrapper::_construct_static)
	];
}

SCRIPT_EXPORT1(DLL_PureScript);

void ISheduledScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ISheduled, CISheduledWrapper>("ISheduled")
	];
}

SCRIPT_EXPORT1(ISheduledScript);

void IRenderableScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<IRenderable, CIRenderableWrapper>("IRenderable")
	];
}

SCRIPT_EXPORT1(IRenderableScript);

void ICollidableScript::script_register	(lua_State *L)
{
	module(L)
	[
		class_<ICollidable>("ICollidable")
			.def(constructor<>())
	];
}

SCRIPT_EXPORT1(ICollidableScript);

void CObjectScript::script_register		(lua_State *L)
{
	module(L)
	[
		class_<CGameObject, bases<DLL_Pure, ISheduled, ICollidable, IRenderable>, CGameObjectWrapper>("CGameObject")
			.def(constructor<>())
			.def("_construct",			&CGameObject::_construct,	&CGameObjectWrapper::_construct_static)
			.def("Visual",				&CGameObject::Visual)

			.def("net_Export",			&CGameObject::net_Export,	&CGameObjectWrapper::net_Export_static)
			.def("net_Import",			&CGameObject::net_Import,	&CGameObjectWrapper::net_Import_static)
			.def("net_Spawn",			&CGameObject::net_Spawn,	&CGameObjectWrapper::net_Spawn_static)

			.def("use",					&CGameObject::use,			&CGameObjectWrapper::use_static)

			.def("getVisible",			&CGameObject::getVisible)
			.def("getEnabled",			&CGameObject::getEnabled)
	];
}

SCRIPT_EXPORT5(CObjectScript, DLL_PureScript, ISheduledScript, ICollidableScript, IRenderableScript);

void IRender_VisualScript::script_register		(lua_State *L)
{
	module(L)
	[
		class_<IRenderVisual>("IRender_Visual")
			.def("dcast_PKinematicsAnimated",&IRenderVisual::dcast_PKinematicsAnimated)
	];
}

SCRIPT_EXPORT1(IRender_VisualScript);

void IKinematicsAnimated_PlayCycle(IKinematicsAnimated* sa, LPCSTR anim)
{
	sa->PlayCycle(anim);
}

void IKinematicsAnimatedScript::script_register		(lua_State *L)
{
	module(L)
	[
		class_<IKinematicsAnimated>("IKinematicsAnimated")
			.def("PlayCycle",		&IKinematicsAnimated_PlayCycle)
	];
}

SCRIPT_EXPORT1(IKinematicsAnimatedScript);

void CBlendScript::script_register		(lua_State *L)
{
	module(L)
		[
			class_<CBlend>("CBlend")
		];
}

SCRIPT_EXPORT1(CBlendScript);

LPCSTR CPatrolPointScript::getName(CPatrolPoint* pp) {
	return pp->name().c_str();
}

void CPatrolPointScript::setName(CPatrolPoint* pp, LPCSTR str) {
	pp->set_name(str);
}

Fvector CPatrolPointScript::getPosition(CPatrolPoint* pp)
{
	return pp->position();
}

void CPatrolPointScript::setPosition(CPatrolPoint* pp, Fvector position)
{
	pp->set_position(position);
}

u32 CPatrolPointScript::getFlags(CPatrolPoint* pp)
{
	return pp->flags();
}

void CPatrolPointScript::setFlags(CPatrolPoint* pp, u32 flags)
{
	pp->set_flags(flags);
}

u32 CPatrolPointScript::getLevelVertex(CPatrolPoint* pp)
{
	return pp->level_vertex_id();
}

void CPatrolPointScript::setLevelVertex(CPatrolPoint* pp, u32 level_vertex)
{
	pp->set_level_vertex(level_vertex);
}

GameGraph::_GRAPH_ID CPatrolPointScript::getGameVertex(CPatrolPoint* pp)
{
	return pp->game_vertex_id();
}

void CPatrolPointScript::setGameVertex(CPatrolPoint* pp, GameGraph::_GRAPH_ID game_vertex)
{
	pp->set_game_vertex(game_vertex);
}

void CPatrolPointScript::script_register(lua_State* L) {
	module(L)[
		class_<CPatrolPoint>("CPatrolPoint")
			.def(constructor<>())
			.property("m_position", &CPatrolPointScript::getPosition, &CPatrolPointScript::setPosition)
			.property("m_flags", &CPatrolPointScript::getFlags, &CPatrolPointScript::setFlags)
			.property("m_level_vertex_id", &CPatrolPointScript::getLevelVertex, &CPatrolPointScript::setLevelVertex)
			.property("m_game_vertex_id", &CPatrolPointScript::getGameVertex, &CPatrolPointScript::setGameVertex)
			.property("m_name", &CPatrolPointScript::getName, &CPatrolPointScript::setName)
			//.def("position", static_cast<CPatrolPoint & (CPatrolPoint::*)(Fvector)>(&CPatrolPoint::position))
	];
}

SCRIPT_EXPORT1(CPatrolPointScript);


void CPatrolPathScript::script_register(lua_State* L) {
	module(L)[
		class_<CPatrolPath>("CPatrolPath")
			.def(constructor<>())
			.def("add_point", &CPatrolPath::add_point)
			.def("point", static_cast<CPatrolPoint(CPatrolPath::*)(u32)>(&CPatrolPath::point))
			.def("add_vertex", &CPatrolPath::add_vertex)
	];
}

SCRIPT_EXPORT1(CPatrolPathScript);
