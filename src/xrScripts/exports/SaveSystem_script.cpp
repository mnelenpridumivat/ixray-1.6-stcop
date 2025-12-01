#include "StdAfx.h"
#include "pch_script.h"
#include "SaveSystem_script.h"
#include "../xrCore/Save/SaveChunk.h"
#include "../xrCore/Save/SaveVariables.h"
#include "../xrCore/Save/SaveObject.h"
#include "../../xrGame/xr_time.h"

namespace CSaveChunk_script {
	bool r_bool(CSaveChunk* Chunk) {
		bool Value;
		Chunk->r_bool(Value);
		return Value;
	}

	/*Fvector r_vec3(CSaveChunk* Chunk) {
		Fvector Value;
		Chunk->r_vec3(Value);
		return Value;
	}*/

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

	/*LPCSTR r_string(CSaveChunk* Chunk) {
		shared_str Value;
		Chunk->r_stringZ(Value);
		return Value.c_str();
	}*/

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

namespace CSaveObject_script {

ISaveObjectStackHandler BeginChunk(ISaveObject* Obj, LPCSTR Name){
	VERIFY(Obj);
	return Obj->BeginChunk(Name);
}

bool HasChunk(ISaveObject* Obj, LPCSTR Name){
	VERIFY(Obj);
	return Obj->HasChunk(Name);
}

void EndChunk(ISaveObject* Obj, ISaveObjectStackHandler handler){
	VERIFY(handler.GetDepth() != u64(-1));
	VERIFY(Obj);
	Obj->EndChunk(handler);
}

void BeginArray(ISaveObject* Obj){
	VERIFY(Obj);
	Obj->BeginArray();
}

void EndArray(ISaveObject* Obj){
	VERIFY(Obj);
	Obj->EndArray();
}

Fvector s_vec3(ISaveObject* Obj, Fvector Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

float s_float(ISaveObject* Obj, double Value) {
	VERIFY(Obj);
	float Casted;
	if (Obj->IsSave()) {
		Casted = Value;
	}
	*Obj << Casted;
	return Casted;
}

u64 s_u64(ISaveObject* Obj, u64 Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

s64 s_s64(ISaveObject* Obj, s64 Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

u32 s_u32(ISaveObject* Obj, u32 Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

s32 s_s32(ISaveObject* Obj, s32 Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

u16 s_u16(ISaveObject* Obj, u16 Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

s16 s_s16(ISaveObject* Obj, s16 Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

u8 s_u8(ISaveObject* Obj, u8 Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

s8 s_s8(ISaveObject* Obj, s8 Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

bool s_bool(ISaveObject* Obj, bool Value) {
	VERIFY(Obj);
	*Obj << Value;
	return Value;
}

LPCSTR s_stringZ(ISaveObject* Obj, LPCSTR Value) {
	VERIFY(Obj);
	shared_str Casted;
	if (Obj->IsSave()) {
		Casted = Value;
	}
	*Obj << Casted;
	return Casted.c_str();
}

bool IsSave(ISaveObject* Obj){
	VERIFY(Obj);
	return Obj->IsSave();
}

}

using namespace luabind;

void SaveSystemScript::script_register(lua_State* L)
{
	module(L)
		[
			class_<ISaveObjectStackHandler>("SaveObjectStackHandler"),
			class_<ISaveObject>("SaveObject")
				.def("BeginChunk", &CSaveObject_script::BeginChunk)
				.def("HasChunk", &CSaveObject_script::HasChunk)
				.def("EndChunk", &CSaveObject_script::EndChunk)
				.def("BeginArray", &CSaveObject_script::BeginArray)
				.def("EndArray", &CSaveObject_script::EndArray)
				.def("s_vec3", &CSaveObject_script::s_vec3)
				.def("s_float", &CSaveObject_script::s_float)
				.def("s_u64", &CSaveObject_script::s_u64)
				.def("s_s64", &CSaveObject_script::s_s64)
				.def("s_u32", &CSaveObject_script::s_u32)
				.def("s_s32", &CSaveObject_script::s_s32)
				.def("s_u16", &CSaveObject_script::s_u16)
				.def("s_s16", &CSaveObject_script::s_s16)
				.def("s_u8", &CSaveObject_script::s_u8)
				.def("s_s8", &CSaveObject_script::s_s8)
				.def("s_bool", &CSaveObject_script::s_bool)
				.def("s_stringZ", &CSaveObject_script::s_stringZ)
				.def("IsSave", &CSaveObject_script::IsSave)
		];
}