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

ISaveObject& CSaveObjectSave::operator<<(float& Value)
{
	GetCurrentChunk()->w_float(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(double& Value)
{
	GetCurrentChunk()->w_double(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(u64& Value)
{
	GetCurrentChunk()->w_u64(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(s64& Value)
{
	GetCurrentChunk()->w_s64(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(u32& Value)
{
	GetCurrentChunk()->w_u32(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(s32& Value)
{
	GetCurrentChunk()->w_s32(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(u16& Value)
{
	GetCurrentChunk()->w_u16(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(s16& Value)
{
	GetCurrentChunk()->w_s16(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(u8& Value)
{
	GetCurrentChunk()->w_u8(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(s8& Value)
{
	GetCurrentChunk()->w_s8(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(bool& Value)
{
	GetCurrentChunk()->w_bool(Value);
	return *this;
}

ISaveObject& CSaveObjectSave::operator<<(LPSTR S)
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

ISaveObject& CSaveObjectLoad::operator<<(float& Value)
{
	GetCurrentChunk()->r_float(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(double& Value)
{
	GetCurrentChunk()->r_double(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(u64& Value)
{
	GetCurrentChunk()->r_u64(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(s64& Value)
{
	GetCurrentChunk()->r_s64(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(u32& Value)
{
	GetCurrentChunk()->r_u32(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(s32& Value)
{
	GetCurrentChunk()->r_s32(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(u16& Value)
{
	GetCurrentChunk()->r_u16(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(s16& Value)
{
	GetCurrentChunk()->r_s16(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(u8& Value)
{
	GetCurrentChunk()->r_u8(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(s8& Value)
{
	GetCurrentChunk()->r_s8(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(bool& Value)
{
	GetCurrentChunk()->r_bool(Value);
	return *this;
}

ISaveObject& CSaveObjectLoad::operator<<(LPSTR S)
{
	GetCurrentChunk()->r_string(S);
	return *this;
}

ISaveObject& operator<<(ISaveObject& Object, char& Value) {
	return Object << (s8&)Value;
}