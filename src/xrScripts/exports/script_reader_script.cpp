////////////////////////////////////////////////////////////////////////////
//	Module 		: script_reader_script.cpp
//	Created 	: 05.10.2004
//  Modified 	: 05.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Script reader
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch_script.h"
#include "script_reader.h"

using namespace luabind;

static bool r_eof_semi(IReader *self_)
{
	return			(!!self_->eof());
}

static LPCSTR r_stringZ_semi(IReader *self_)
{
	shared_str		temp;
	self_->r_stringZ	(temp);
	return			(*temp);
}

static bool r_bool_semi(IReader *self_)
{
	return			(!!self_->r_u8());
}

static void r_fvector3_semi(IReader *self_, Fvector *arg0)
{
	self_->r_fvector3(*arg0);
}

#pragma optimize("s",on)
void CScriptReader::script_register(lua_State *L)
{
	module(L)
	[
		class_<IReader>("reader")
			.def("r_seek",			&IReader::seek			)
			.def("r_tell",			&IReader::tell			)
			.def("r_vec3",			&::r_fvector3_semi		)
			.def("r_bool",			&r_bool_semi			)
			.def("r_float",			&IReader::r_float		)
			.def("r_u64",			&IReader::r_u64			)
			.def("r_s64",			&IReader::r_s64			)
			.def("r_u32",			&IReader::r_u32			)
			.def("r_s32",			&IReader::r_s32			)
			.def("r_u16",			&IReader::r_u16			)
			.def("r_s16",			&IReader::r_s16			)
			.def("r_u8",			&IReader::r_u8			)
			.def("r_s8",			&IReader::r_s8			)
			.def("r_float_q16",		&IReader::r_float_q16	)
			.def("r_float_q8",		&IReader::r_float_q8	)
			.def("r_angle16",		&IReader::r_angle16		)
			.def("r_angle8",		&IReader::r_angle8		)
			.def("r_dir",			&IReader::r_dir			)
			.def("r_sdir",			&IReader::r_sdir		)
			.def("r_stringZ",		&r_stringZ_semi			)
			.def("r_elapsed",		&IReader::elapsed		)
			.def("r_advance",		&IReader::advance		)
			.def("r_eof",			&r_eof_semi				)
	];
}

SCRIPT_EXPORT1(CScriptReader);
