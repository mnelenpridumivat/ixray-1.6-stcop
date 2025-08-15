#include "stdafx.h"
#include "SaveObject.h"
#include "SaveManager.h"
#include "SaveChunkHandle.h"

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

#ifndef MASTER_GOLD
void CSaveObject::ClearDebugData()
{
	while (!_debugTopChunkNamesQueue.empty())
	{
		_debugTopChunkNamesQueue.pop();
	}
}

void CSaveObject::PopDebugData()
{
	Msg("The history of top chunk:");
	while (!_debugTopChunkNamesQueue.empty())
	{
		xr_string Top = _debugTopChunkNamesQueue.front().c_str();
		_debugTopChunkNamesQueue.pop();
		Top = "\t" + Top;
		Msg(Top.c_str());
	}
	Msg("End of the history of top chunk");
}
#endif

void CSaveObject::EndChunk(ISaveObjectStackHandler handler)
{
	VERIFY(handler.GetDepth() != u64(-1));
	VERIFY(!_chunkStack.empty());
	xr_string chunk = _chunkStack.top()->GetChunkName();
	_chunkStack.pop();
	R_ASSERT3(handler.GetDepth() == _chunkStack.size(), "Chunk has invalid closing tags!", chunk.c_str());
#ifndef MASTER_GOLD
	_debugTopChunkNamesQueue.push(_chunkStack.top()->GetChunkName());
#endif
}

void CSaveObject::EndArray()
{
	GetCurrentChunk()->EndArray();
}

bool CSaveObject::HasChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	return _chunkStack.top()->ContainsSubchunk(ChunkName);
}

/*void CSaveObject::MarkDirty() {
	_dirty = true;
	if (!_handles.size()) {
		xr_delete(this);
	}
}*/

/*void CSaveObject::NotifyHandleDestruction(ISaveChunkHandleInterface* handle)
{
	VERIFY(_handles.find(handle) != _handles.end());
	_handles.erase(handle);
	xr_delete(handle);
	if (_dirty && !_handles.size()) {
		xr_delete(this);
	}
}*/

u64 CSaveObjectLoad::ExtractCurrentChunk()
{
	auto CurrentChunk = _chunkStack.top();
	auto Result = new CSaveChunkHandle(this, CurrentChunk);
	auto ID = CSaveManager::GetInstance().RegisterHandle(Result);
	return ID;
}

void CSaveObjectLoad::MergeChunkByHandle(ISaveChunkHandleInterface* handle)
{
	VERIFY(false, "Attempt to copy chunk into load object!");
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

ISaveObjectStackHandler CSaveObjectSave::BeginChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->BeginChunk(ChunkName));
#ifndef MASTER_GOLD
	_debugTopChunkNamesQueue.push(ChunkName);
#endif
	return ISaveObjectStackHandler(_chunkStack.size()-1);
}

void CSaveObjectSave::BeginArray()
{
	GetCurrentChunk()->WriteArray();
}

u64 CSaveObjectSave::ExtractCurrentChunk()
{
	VERIFY(false, "Cannot extract chunk from saving object!");
}

void CSaveObjectSave::MergeChunkByHandle(ISaveChunkHandleInterface* handle)
{
	auto InsertingChunk = handle->GetChunk();
	auto CurrentChunk = GetCurrentChunk();
	CurrentChunk->CopySubchunks(InsertingChunk);
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

ISaveObject& CSaveObjectSave::operator<<(shared_str& S)
{
	GetCurrentChunk()->w_string(S);
	return *this;
}

void CSaveObjectSave::Write(CMemoryBuffer* buffer, SSaveTask* Task)
{
	_rootChunk->Write(*buffer, Task);
}

CSaveObjectLoad::CSaveObjectLoad()
{
	_rootChunk = new CSaveChunk("Root");
	_chunkStack.push(_rootChunk);
}

CSaveObjectLoad::CSaveObjectLoad(ISaveChunkHandleInterface* Root)
{
	_rootChunk = Root->GetChunk();
	_isPartial = true;
	_chunkStack.push(_rootChunk);
}

ISaveObjectStackHandler CSaveObjectLoad::BeginChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
#ifndef MASTER_GOLD
	_debugTopChunkNamesQueue.push(ChunkName);
#endif
	_chunkStack.push(_chunkStack.top()->FindChunk(ChunkName));
	return ISaveObjectStackHandler(_chunkStack.size()-1);
}

void CSaveObjectLoad::BeginArray()
{
	u64 ArrSize;
	GetCurrentChunk()->ReadArray(ArrSize);
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

ISaveObject& CSaveObjectLoad::operator<<(shared_str& S)
{
	GetCurrentChunk()->r_string(S);
	return *this;
}

void CSaveObjectLoad::Parse(IReader* stream)
{
	{
		ESaveVariableType type;
		stream->r(&type, sizeof(ESaveVariableType));
		VERIFY(type == ESaveVariableType::t_chunk);
	}
	{
		ESaveVariableType type;
		stream->r(&type, sizeof(ESaveVariableType));
		VERIFY(type == ESaveVariableType::t_chunkStart);
	}
	shared_str chunkName;
	CSaveManager::GetInstance().ConditionalReadString(stream, chunkName);
	VERIFY(chunkName == "Root");
	GetCurrentChunk()->Parse(stream);
}

ISaveObject& operator<<(ISaveObject& Object, char& Value) {
	return Object << (s8&)Value;
}

XRCORE_API ISaveObject& operator<<(ISaveObject& Object, LPSTR& Value)
{
	if (Object.IsSave()) {
		shared_str temp = Value;
		Object << temp;
	}
	else {
		xr_free(Value);
		shared_str temp;
		Object << temp;
		Value = xr_strdup(temp.c_str());
		//Value = (LPSTR)temp.c_str();
	}
	return Object;
}
