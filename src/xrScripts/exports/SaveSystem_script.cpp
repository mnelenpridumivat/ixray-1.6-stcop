#include "StdAfx.h"
#include "pch_script.h"
#include "SaveSystem_script.h"
#include "Save/SaveChunk.h"
#include "Save/SaveVariables.h"
#include "Save/SaveObject.h"
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

void BeginChunk(ISaveObject* Obj, LPCSTR Name){
	VERIFY(Obj);
	Obj->BeginChunk(Name);
}

void EndChunk(ISaveObject* Obj){
	VERIFY(Obj);
	Obj->EndChunk();
}

void BeginArray(ISaveObject* Obj){
	VERIFY(Obj);
	Obj->BeginArray();
}

void EndArray(ISaveObject* Obj){
	VERIFY(Obj);
	Obj->EndArray();
}

/*Fvector s_vec3(ISaveObject* Obj, luabind::object Value) {
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TUSERDATA);
	Fvector Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<Fvector>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

float s_float(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	float Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<float>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

u64 s_u64(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	u64 Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<u64>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

s64 s_s64(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	s64 Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<s64>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

u32 s_u32(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	u32 Casted;
	if (Obj->IsSave()) {
		auto tCasted = object_cast<double>(Value);
		VERIFY(tCasted >= std::numeric_limits<u32>::min());
		VERIFY(tCasted <= std::numeric_limits<u32>::max());
		Casted = tCasted;
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

s32 s_s32(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	s32 Casted;
	if (Obj->IsSave()) {
		auto tCasted = object_cast<double>(Value);
		VERIFY(tCasted >= std::numeric_limits<s32>::min());
		VERIFY(tCasted <= std::numeric_limits<s32>::max());
		Casted = tCasted;
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

u16 s_u16(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	u16 Casted;
	if (Obj->IsSave()) {
		auto tCasted = object_cast<double>(Value);
		VERIFY(tCasted >= std::numeric_limits<u16>::min());
		VERIFY(tCasted <= std::numeric_limits<u16>::max());
		Casted = tCasted;
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

s16 s_s16(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	s16 Casted;
	if (Obj->IsSave()) {
		auto tCasted = object_cast<double>(Value);
		VERIFY(tCasted >= std::numeric_limits<s16>::min());
		VERIFY(tCasted <= std::numeric_limits<s16>::max());
		Casted = tCasted;
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

u8 s_u8(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	u8 Casted;
	if (Obj->IsSave()) {
		auto tCasted = object_cast<double>(Value);
		VERIFY(tCasted >= std::numeric_limits<u8>::min());
		VERIFY(tCasted <= std::numeric_limits<u8>::max());
		Casted = tCasted;
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

s8 s_s8(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	s8 Casted;
	if (Obj->IsSave()) {
		auto tCasted = object_cast<double>(Value);
		VERIFY(tCasted >= std::numeric_limits<s8>::min());
		VERIFY(tCasted <= std::numeric_limits<s8>::max());
		Casted = tCasted;
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

bool s_bool(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TBOOLEAN);
	bool Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<bool>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted;
}

LPCSTR s_stringZ(ISaveObject* Obj, luabind::object Value){
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TSTRING);
	shared_str Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<LPCSTR>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		//Value = Casted;
	}
	return Casted.c_str();
}*/

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

/*CSaveObjectSave* GetAsSave(ISaveObject* Obj) {
	auto Casted = smart_cast<CSaveObjectSave*>(Obj);
	VERIFY(Casted);
	return Casted;
}

CSaveObjectLoad* GetAsLoad(ISaveObject* Obj) {
	auto Casted = smart_cast<CSaveObjectLoad*>(Obj);
	VERIFY(Casted);
	return Casted;
}*/

}

using namespace luabind;

void SaveSystemScript::script_register(lua_State* L)
{
	module(L)
		[
			class_<ISaveObject>("SaveObject")
				.def("BeginChunk", &CSaveObject_script::BeginChunk)
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
			/*class_<CSaveObjectSave, ISaveObject>("SaveObjectSave")
				//.def("GetCurrentChunk", &CSaveObject::GetCurrentChunk)
				.def("BeginChunk", &CSaveObjectSave::BeginChunk)
				.def("EndChunk", &CSaveObject::EndChunk)
				.def("BeginArray", &CSaveObjectSave::BeginArray)
				.def("EndArray", &CSaveObject::EndArray)
				.def("s_vec3", &CSaveObject_script::w_vec3)
				.def("s_float", &CSaveObject_script::w_float)
				//.def("s_u64", &CSaveObject_script::w_u64)
				//.def("s_s64", &CSaveObject_script::w_s64)
				.def("s_u32", &CSaveObject_script::w_u32)
				.def("s_s32", &CSaveObject_script::w_s32)
				.def("s_u16", &CSaveObject_script::w_u16)
				.def("s_s16", &CSaveObject_script::w_s16)
				.def("s_u8", &CSaveObject_script::w_u8)
				.def("s_s8", &CSaveObject_script::w_s8)
				.def("s_bool", &CSaveObject_script::w_bool)
				.def("s_stringZ", &CSaveObject_script::w_string)
				//.def("s_CTime", &CSaveObject_script::w_CTime)
				.def("IsSave", &CSaveObjectSave::IsSave),
			class_<CSaveObjectLoad, ISaveObject>("SaveObjectLoad")
				//.def("GetCurrentChunk", &CSaveObject::GetCurrentChunk)
				.def("BeginChunk", &CSaveObjectLoad::BeginChunk)
				.def("EndChunk", &CSaveObject::EndChunk)
				.def("BeginArray", &CSaveObjectLoad::BeginArray)
				.def("EndArray", &CSaveObject::EndArray)
				.def("s_vec3", &CSaveObjectLoad::r_vec3, pure_out_value<2>())
				.def("s_float", &CSaveObjectLoad::r_float, pure_out_value<2>())
				//.def("s_u64", &CSaveObject_script::r_u64, pure_out_value<2>())
				//.def("s_s64", &CSaveObject_script::r_s64, pure_out_value<2>())
				.def("s_u32", &CSaveObjectLoad::r_u32, pure_out_value<2>())
				.def("s_s32", &CSaveObjectLoad::r_s32, pure_out_value<2>())
				.def("s_u16", &CSaveObjectLoad::r_u16, pure_out_value<2>())
				.def("s_s16", &CSaveObjectLoad::r_s16, pure_out_value<2>())
				.def("s_u8", &CSaveObjectLoad::r_u8, pure_out_value<2>())
				.def("s_s8", &CSaveObjectLoad::r_s8, pure_out_value<2>())
				.def("s_bool", &CSaveObjectLoad::r_bool, pure_out_value<2>())
				.def("s_stringZ", &CSaveObjectLoad::r_string, pure_out_value<2>())
				//.def("s_CTime", &CSaveObject_script::r_CTime, pure_out_value<2>())
				.def("IsSave", &CSaveObjectLoad::IsSave)*/
		];
	/*module(L, "SaveObjectConverter")
		[
			def("AsSave", &CSaveObject_script::GetAsSave),
			def("AsLoad", &CSaveObject_script::GetAsLoad)
		];*/
}

SCRIPT_EXPORT1(SaveSystemScript);

/*void ISaveObjectWrapper::BeginChunk(LPCSTR Name)
{
	VERIFY(Obj);
	Obj->BeginChunk(Name);
}

void ISaveObjectWrapper::EndChunk()
{
	VERIFY(Obj);
	Obj->EndChunk();
}

void ISaveObjectWrapper::BeginArray()
{
	VERIFY(Obj);
	Obj->BeginArray();
}

void ISaveObjectWrapper::EndArray()
{
	VERIFY(Obj);
	Obj->EndArray();
}

void ISaveObjectWrapper::s_vec3(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TUSERDATA);
	Fvector Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<Fvector>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_float(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	float Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<float>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_u64(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	u64 Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<u64>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_s64(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	s64 Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<s64>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_u32(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	if (Obj->IsSave()) {
		auto Casted = object_cast<u64>(Value);
		VERIFY(Casted < std::numeric_limits<u32>::min());
		VERIFY(Casted > std::numeric_limits<u32>::max());
		u32 tCasted = Casted;
		*Obj << tCasted;
	}
	else {
		u32 Casted;
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_s32(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	if (Obj->IsSave()) {
		auto Casted = object_cast<s64>(Value);
		VERIFY(Casted < std::numeric_limits<s32>::min());
		VERIFY(Casted > std::numeric_limits<s32>::max());
		s32 tCasted = Casted;
		*Obj << tCasted;
	}
	else {
		s32 Casted;
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_u16(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	if (Obj->IsSave()) {
		auto Casted = object_cast<u64>(Value);
		VERIFY(Casted < std::numeric_limits<u16>::min());
		VERIFY(Casted > std::numeric_limits<u16>::max());
		u16 tCasted = Casted;
		*Obj << tCasted;
	}
	else {
		u16 Casted;
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_s16(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	if (Obj->IsSave()) {
		auto Casted = object_cast<s64>(Value);
		VERIFY(Casted < std::numeric_limits<s16>::min());
		VERIFY(Casted > std::numeric_limits<s16>::max());
		s16 tCasted = Casted;
		*Obj << tCasted;
	}
	else {
		s16 Casted;
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_u8(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	if (Obj->IsSave()) {
		auto Casted = object_cast<u64>(Value);
		VERIFY(Casted < std::numeric_limits<u8>::min());
		VERIFY(Casted > std::numeric_limits<u8>::max());
		u8 tCasted = Casted;
		*Obj << tCasted;
	}
	else {
		u8 Casted;
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_s8(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TNUMBER);
	if (Obj->IsSave()) {
		auto Casted = object_cast<s64>(Value);
		VERIFY(Casted < std::numeric_limits<s8>::min());
		VERIFY(Casted > std::numeric_limits<s8>::max());
		s8 tCasted = Casted;
		*Obj << tCasted;
	}
	else {
		s8 Casted;
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_bool(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TBOOLEAN);
	bool Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<bool>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		Value = Casted;
	}
}

void ISaveObjectWrapper::s_stringZ(luabind::object Value)
{
	VERIFY(Obj);
	VERIFY(Value.type() == LUA_TSTRING);
	shared_str Casted;
	if (Obj->IsSave()) {
		Casted = object_cast<LPCSTR>(Value);
		*Obj << Casted;
	}
	else {
		*Obj << Casted;
		Value = Casted;
	}
}

bool ISaveObjectWrapper::IsSave()
{
	VERIFY(Obj);
	return Obj->IsSave();
}*/
