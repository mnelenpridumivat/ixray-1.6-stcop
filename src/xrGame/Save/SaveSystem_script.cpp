#include "StdAfx.h"
#include "SaveSystem_script.h"
#include "SaveChunk.h"
#include "SaveVariables.h"

namespace CSaveChunk_script {
	bool r_bool(CSaveChunk* Chunk) {
		bool Value;
		Chunk->r_bool(Value);
		return Value;
	}

	Fvector r_vec3(CSaveChunk* Chunk) {
		Fvector Value;
		Chunk->r_vec3(Value);
		return Value;
	}

	float r_float(CSaveChunk* Chunk) {
		float Value;
		Chunk->r_float(Value);
		return Value;
	}

	u64 r_u64(CSaveChunk* Chunk) {
		u64 Value;
		Chunk->r_u64(Value);
		return Value;
	}

	s64 r_s64(CSaveChunk* Chunk) {
		s64 Value;
		Chunk->r_s64(Value);
		return Value;
	}

	u32 r_u32(CSaveChunk* Chunk) {
		u32 Value;
		Chunk->r_u32(Value);
		return Value;
	}

	s32 r_s32(CSaveChunk* Chunk) {
		s32 Value;
		Chunk->r_s32(Value);
		return Value;
	}

	u16 r_u16(CSaveChunk* Chunk) {
		u16 Value;
		Chunk->r_u16(Value);
		return Value;
	}

	s16 r_s16(CSaveChunk* Chunk) {
		s16 Value;
		Chunk->r_s16(Value);
		return Value;
	}

	u8 r_u8(CSaveChunk* Chunk) {
		u8 Value;
		Chunk->r_u8(Value);
		return Value;
	}

	s8 r_s8(CSaveChunk* Chunk) {
		s8 Value;
		Chunk->r_s8(Value);
		return Value;
	}

	LPCSTR r_string(CSaveChunk* Chunk) {
		shared_str Value;
		Chunk->r_stringZ(Value);
		return Value.c_str();
	}

	u64 ReadArray(CSaveChunk* Chunk) {
		u64 Size;
		Chunk->ReadArray(Size);
		return Size;
	}

	CSaveChunk* BeginChunk(CSaveChunk* Chunk, LPCSTR ChunkName) {
		return Chunk->BeginChunk(ChunkName);
	}

	CSaveChunk* FindChunk(CSaveChunk* Chunk, LPCSTR ChunkName) {
		return Chunk->FindChunk(ChunkName);
	}

}

using namespace luabind;

void SaveSystemScript::script_register(lua_State* L)
{
	module(L)
		[
			class_<CSaveChunk>("Serialisation")
				.def("w_vec3", &CSaveChunk::w_vec3)
				.def("w_float", &CSaveChunk::w_float)
				.def("w_u64", &CSaveChunk::w_u64)
				.def("w_s64", &CSaveChunk::w_s64)
				.def("w_u32", &CSaveChunk::w_u32)
				.def("w_s32", &CSaveChunk::w_s32)
				.def("w_u16", &CSaveChunk::w_u16)
				.def("w_s16", &CSaveChunk::w_s16)
				.def("w_u8", &CSaveChunk::w_u8)
				.def("w_s8", &CSaveChunk::w_s8)
				.def("w_bool", &CSaveChunk::w_bool)
				.def("w_string", &CSaveChunk::w_string)

				.def("r_vec3", &CSaveChunk_script::r_vec3)
				.def("r_bool", &CSaveChunk_script::r_bool)
				.def("r_float", &CSaveChunk_script::r_float)
				.def("r_u64", &CSaveChunk_script::r_u64)
				.def("r_s64", &CSaveChunk_script::r_s64)
				.def("r_u32", &CSaveChunk_script::r_u32)
				.def("r_s32", &CSaveChunk_script::r_s32)
				.def("r_u16", &CSaveChunk_script::r_u16)
				.def("r_s16", &CSaveChunk_script::r_s16)
				.def("r_u8", &CSaveChunk_script::r_u8)
				.def("r_s8", &CSaveChunk_script::r_s8)
				.def("r_string", &CSaveChunk_script::r_string)

				.def("ReadArray", &CSaveChunk_script::ReadArray)
				.def("WriteArray", &CSaveChunk::WriteArray)
				.def("EndArray", &CSaveChunk::EndArray)

				.def("BeginChunk", &CSaveChunk_script::BeginChunk)
				.def("FindChunk", &CSaveChunk_script::FindChunk)
		];
}