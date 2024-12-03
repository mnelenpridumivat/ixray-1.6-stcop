#include "StdAfx.h"
#include "SaveSystem_script.h"
#include "SaveChunk.h"
#include "SaveVariables.h"

using namespace luabind;

void SaveSystemScript::script_register(lua_State* L)
{
	/*module(L)
		[
			class_<CSaveChunk>("Serialisation")
				.def("w_vec3", &NET_Packet::w_vec3)
				.def("w_float", &NET_Packet::w_float)
				.def("w_u64", &NET_Packet::w_u64)
				.def("w_s64", &NET_Packet::w_s64)
				.def("w_u32", &NET_Packet::w_u32)
				.def("w_s32", &NET_Packet::w_s32)
				.def("w_u16", &NET_Packet::w_u16)
				.def("w_s16", &NET_Packet::w_s16)
				.def("w_u8", &NET_Packet::w_u8)
				//			.def("w_s8",			&NET_Packet::w_s8			)
				.def("w_bool", &w_bool)
				.def("w_float_q16", &NET_Packet::w_float_q16)
				.def("w_float_q8", &NET_Packet::w_float_q8)
				.def("w_angle16", &NET_Packet::w_angle16)
				.def("w_angle8", &NET_Packet::w_angle8)
				.def("w_dir", &NET_Packet::w_dir)
				.def("w_sdir", &NET_Packet::w_sdir)
				.def("w_stringZ", (void (NET_Packet::*)(LPCSTR))(&NET_Packet::w_stringZ))
				.def("w_matrix", &NET_Packet::w_matrix)
				.def("w_clientID", &NET_Packet::w_clientID)
				.def("r_vec3", (void (NET_Packet::*)(Fvector&))(&NET_Packet::r_vec3), pure_out_value<2>())
				.def("r_bool", &r_bool)
				.def("r_float", (float	(NET_Packet::*)())(&NET_Packet::r_float))
				.def("r_u64", (u64(NET_Packet::*)())(&NET_Packet::r_u64))
				.def("r_s64", (s64(NET_Packet::*)())(&NET_Packet::r_s64))
				.def("r_u32", (u32(NET_Packet::*)())(&NET_Packet::r_u32))
				.def("r_s32", (s32(NET_Packet::*)())(&NET_Packet::r_s32))
				.def("r_u16", (u16(NET_Packet::*)())(&NET_Packet::r_u16))
				.def("r_s16", (s16(NET_Packet::*)())(&NET_Packet::r_s16))
				.def("r_u8", (u8(NET_Packet::*)())(&NET_Packet::r_u8))
				.def("r_s8", (s8(NET_Packet::*)())(&NET_Packet::r_s8))
				.def("r_angle16", &NET_Packet::r_angle16, out_value<2>())
				.def("r_angle8", &NET_Packet::r_angle8, out_value<2>())
				.def("r_dir", &NET_Packet::r_dir)
				.def("r_sdir", &NET_Packet::r_sdir)
				.def("r_stringZ", &r_stringZ)
				.def("r_matrix", &NET_Packet::r_matrix)
				.def("r_clientID", &r_clientID)
		];*/
}