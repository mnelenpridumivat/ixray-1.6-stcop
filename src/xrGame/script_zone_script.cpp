////////////////////////////////////////////////////////////////////////////
//	Module 		: script_zone_script.cpp
//	Created 	: 10.10.2003
//  Modified 	: 11.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Script zone object script export
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "pch_script.h"
#include "SamZone.h"
#include "script_zone.h"
#include "smart_zone.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptZone::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptZone,DLL_Pure>("ce_script_zone")
			.def(constructor<>())
	];
}

SCRIPT_EXPORT2(CScriptZone, DLL_PureScript);

void CSmartZone::script_register(lua_State *L)
{
	module(L)
	[
		class_<CSmartZone,DLL_Pure>("ce_smart_zone")
			.def(constructor<>()),
		class_<CSamZone, CSmartZone>("CSamZone")
			.def(constructor<>())
	];
}

SCRIPT_EXPORT2(CSmartZone, DLL_PureScript);
