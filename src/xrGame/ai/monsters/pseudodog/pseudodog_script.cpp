#include "stdafx.h"
#include "pch_script.h"
#include "pseudodog.h"
#include "psy_dog.h"

using namespace luabind;

#pragma optimize("s",on)
void CAI_PseudoDog::script_register(lua_State *L)
{
	module(L)
	[
		class_<CAI_PseudoDog,CGameObject>("CAI_PseudoDog")
			.def(constructor<>())
	];
}

SCRIPT_EXPORT2(CAI_PseudoDog, CObjectScript);

void CPsyDog::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPsyDog,CGameObject>("CPsyDog")
			.def(constructor<>())
		];
}

SCRIPT_EXPORT2(CPsyDog, CObjectScript);

void CPsyDogPhantom::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPsyDogPhantom,CGameObject>("CPsyDogPhantom")
			.def(constructor<>())
		];
}

SCRIPT_EXPORT2(CPsyDogPhantom, CObjectScript);
