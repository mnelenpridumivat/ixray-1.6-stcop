#include "stdafx.h"
#include "SaveObject.h"
#include "SaveManager.h"

CSaveObject::CSaveObject()
{
	_rootChunk = new CSaveChunk("Root");
	_chunkStack.push(_rootChunk);
}

CSaveObject::CSaveObject(CSaveChunk* Root)
{
	_rootChunk = Root;
	_isPartial = true;
	_chunkStack.push(_rootChunk);
}

CSaveObject::~CSaveObject()
{
	if (!_isPartial) {
		xr_delete(_rootChunk);
	}
}

CSaveChunk* CSaveObject::GetCurrentChunk()
{
	VERIFY(!_chunkStack.empty());
	return _chunkStack.top();
}

/*void CSaveObject::BeginChunk(shared_str ChunkName)
{
	VERIFY2(false, "Do not use base class CSaveObject directly!");
}

void CSaveObject::FindChunk(shared_str ChunkName)
{
	VERIFY2(false, "Do not use base class CSaveObject directly!");
}*/

void CSaveObject::EndChunk()
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.pop();
}

CSaveObjectSave::CSaveObjectSave()
{
	//_rootChunk = new CSaveChunk("Root");
	//_chunkStack.push(_rootChunk);
}

CSaveObjectSave::CSaveObjectSave(CSaveChunk* Root)
{
	//_rootChunk = Root;
	//_isPartial = true;
	//_chunkStack.push(_rootChunk);
}

void CSaveObjectSave::BeginChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->BeginChunk(ChunkName));
}

/*void CSaveObjectSave::s_vec3(Fvector& Value)
{
	GetCurrentChunk()->w_vec3(Value);
}

void CSaveObjectSave::s_vec4(Fvector4& a)
{
	GetCurrentChunk()->w_vec4(a);
}

void CSaveObjectSave::s_quat(Fquaternion& Value)
{
	GetCurrentChunk()->w_quat(Value);
}

void CSaveObjectSave::s_float(float& Value)
{
	GetCurrentChunk()->w_float(Value);
}

void CSaveObjectSave::s_u64(u64& Value)
{
	GetCurrentChunk()->w_u64(Value);
}

void CSaveObjectSave::s_s64(s64& Value)
{
	GetCurrentChunk()->w_s64(Value);
}

void CSaveObjectSave::s_u32(u32& Value)
{
	GetCurrentChunk()->w_u32(Value);
}

void CSaveObjectSave::s_s32(s32& Value)
{
	GetCurrentChunk()->w_s32(Value);
}

void CSaveObjectSave::s_u16(u16& Value)
{
	GetCurrentChunk()->w_u16(Value);
}

void CSaveObjectSave::s_s16(s16& Value)
{
	GetCurrentChunk()->w_s16(Value);
}

void CSaveObjectSave::s_u8(u8& Value)
{
	GetCurrentChunk()->w_u8(Value);
}

void CSaveObjectSave::s_s8(s8& Value)
{
	GetCurrentChunk()->w_s8(Value);
}

void CSaveObjectSave::s_bool(bool& Value)
{
	GetCurrentChunk()->w_bool(Value);
}

void CSaveObjectSave::s_string(LPSTR S)
{
	GetCurrentChunk()->w_string(S);
}

void CSaveObjectSave::s_stringZ(shared_str& p)
{
	GetCurrentChunk()->w_stringZ(p);
}

void CSaveObjectSave::s_stringZ(xr_string& p)
{
	GetCurrentChunk()->w_stringZ(p);
}

void CSaveObjectSave::s_matrix(Fmatrix& M)
{
	GetCurrentChunk()->w_matrix(M);
}

void CSaveObjectSave::s_clientID(ClientID& C)
{
	GetCurrentChunk()->w_clientID(C);
}

void CSaveObjectSave::s_stringZ_s(LPSTR string, u32 size)
{
	GetCurrentChunk()->w_stringZ(string);
}*/

CSaveObject& CSaveObjectSave::operator<<(float& Value)
{
	GetCurrentChunk()->w_float(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(double& Value)
{
	GetCurrentChunk()->w_double(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(u64& Value)
{
	GetCurrentChunk()->w_u64(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(s64& Value)
{
	GetCurrentChunk()->w_s64(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(u32& Value)
{
	GetCurrentChunk()->w_u32(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(s32& Value)
{
	GetCurrentChunk()->w_s32(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(u16& Value)
{
	GetCurrentChunk()->w_u16(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(s16& Value)
{
	GetCurrentChunk()->w_s16(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(u8& Value)
{
	GetCurrentChunk()->w_u8(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(s8& Value)
{
	GetCurrentChunk()->w_s8(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(bool& Value)
{
	GetCurrentChunk()->w_bool(Value);
	return *this;
}

CSaveObject& CSaveObjectSave::operator<<(LPSTR S)
{
	GetCurrentChunk()->w_string(S);
	return *this;
}

void CSaveObjectSave::Write(CMemoryBuffer* buffer)
{
	_rootChunk->Write(*buffer);
}

CSaveObjectLoad::CSaveObjectLoad()
{
	_rootChunk = new CSaveChunk("Root");
	_chunkStack.push(_rootChunk);
}

CSaveObjectLoad::CSaveObjectLoad(CSaveChunk* Root)
{
	_rootChunk = Root;
	_isPartial = true;
	_chunkStack.push(_rootChunk);
}

void CSaveObjectLoad::BeginChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->FindChunk(ChunkName));
}

/*void CSaveObjectLoad::s_vec3(Fvector& Value)
{
	GetCurrentChunk()->r_vec3(Value);
}

void CSaveObjectLoad::s_vec4(Fvector4& a)
{
	GetCurrentChunk()->r_vec4(a);
}

void CSaveObjectSave::s_quat(Fquaternion& Value)
{
	GetCurrentChunk()->r_quat(Value);
}

void CSaveObjectLoad::s_float(float& Value)
{
	GetCurrentChunk()->r_float(Value);
}

void CSaveObjectLoad::s_u64(u64& Value)
{
	GetCurrentChunk()->r_u64(Value);
}

void CSaveObjectLoad::s_s64(s64& Value)
{
	GetCurrentChunk()->r_s64(Value);
}

void CSaveObjectLoad::s_u32(u32& Value)
{
	GetCurrentChunk()->r_u32(Value);
}

void CSaveObjectLoad::s_s32(s32& Value)
{
	GetCurrentChunk()->r_s32(Value);
}

void CSaveObjectLoad::s_u16(u16& Value)
{
	GetCurrentChunk()->r_u16(Value);
}

void CSaveObjectLoad::s_s16(s16& Value)
{
	GetCurrentChunk()->r_s16(Value);
}

void CSaveObjectLoad::s_u8(u8& Value)
{
	GetCurrentChunk()->r_u8(Value);
}

void CSaveObjectLoad::s_s8(s8& Value)
{
	GetCurrentChunk()->r_s8(Value);
}

void CSaveObjectLoad::s_bool(bool& Value)
{
	GetCurrentChunk()->r_bool(Value);
}

void CSaveObjectLoad::s_string(LPSTR S)
{
	GetCurrentChunk()->r_stringZ(S);
}

void CSaveObjectLoad::s_stringZ(shared_str& p)
{
	GetCurrentChunk()->r_stringZ(p);
}

void CSaveObjectLoad::s_stringZ(xr_string& p)
{
	GetCurrentChunk()->r_stringZ(p);
}

void CSaveObjectLoad::s_matrix(Fmatrix& M)
{
	GetCurrentChunk()->r_matrix(M);
}

void CSaveObjectLoad::s_clientID(ClientID& C)
{
	GetCurrentChunk()->r_clientID(C);
}

void CSaveObjectLoad::s_stringZ_s(LPSTR string, u32 size)
{
	GetCurrentChunk()->r_stringZ_s(string, size);
}*/

CSaveObject& CSaveObjectLoad::operator<<(float& Value)
{
	GetCurrentChunk()->r_float(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(double& Value)
{
	GetCurrentChunk()->r_double(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(u64& Value)
{
	GetCurrentChunk()->r_u64(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(s64& Value)
{
	GetCurrentChunk()->r_s64(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(u32& Value)
{
	GetCurrentChunk()->r_u32(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(s32& Value)
{
	GetCurrentChunk()->r_s32(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(u16& Value)
{
	GetCurrentChunk()->r_u16(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(s16& Value)
{
	GetCurrentChunk()->r_s16(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(u8& Value)
{
	GetCurrentChunk()->r_u8(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(s8& Value)
{
	GetCurrentChunk()->r_s8(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(bool& Value)
{
	GetCurrentChunk()->r_bool(Value);
	return *this;
}

CSaveObject& CSaveObjectLoad::operator<<(LPSTR S)
{
	GetCurrentChunk()->r_string(S);
	return *this;
}
