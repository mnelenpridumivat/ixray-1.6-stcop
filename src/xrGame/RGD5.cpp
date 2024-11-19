#include "StdAfx.h"
#include "pch_script.h"
#include "RGD5.h"

CRGD5::CRGD5(void)
{
}

CRGD5::~CRGD5(void)
{
}

using namespace luabind;

#pragma optimize("s",on)
void CRGD5::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CRGD5,CGameObject>("CRGD5")
			.def(constructor<>())
	];
}

SCRIPT_EXPORT2(CRGD5, CObjectScript);
