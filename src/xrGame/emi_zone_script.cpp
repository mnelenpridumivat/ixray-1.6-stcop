#include "stdafx.h"
#include "pch_script.h"
#include "EmiZone.h"
#include "base_client_classes.h"

using namespace luabind;

#pragma optimize("s",on)
void CEmiZone::script_register(lua_State* L)
{
	module(L)
		[
			class_<CEmiZone, CGameObject>("CEmiZone")
			.def(constructor<>())
		];
}

SCRIPT_EXPORT2(CEmiZone, CObjectScript);
