#pragma once
#include "SaveChunkHandleInterface.h"
#include "xrCore.h"

class shared_str;

class XRCORE_API ISaveObject {
public:
	virtual void BeginChunk(shared_str ChunkName) = 0;
	virtual void EndChunk() = 0;
	virtual void BeginArray() = 0;
	virtual void EndArray() = 0;

	virtual bool HasChunk(shared_str ChunkName) = 0;

	virtual bool IsSave() = 0;

	virtual u64 ExtractCurrentChunk() = 0;
	virtual u64 GetChunkStackDepth() = 0;

	virtual ISaveObject& operator<<(float& Value) = 0;
	virtual ISaveObject& operator<<(double& Value) = 0;
	virtual ISaveObject& operator<<(u64& Value) = 0;
	virtual ISaveObject& operator<<(s64& Value) = 0;
	virtual ISaveObject& operator<<(u32& Value) = 0;
	virtual ISaveObject& operator<<(s32& Value) = 0;
	virtual ISaveObject& operator<<(u16& Value) = 0;
	virtual ISaveObject& operator<<(s16& Value) = 0;
	virtual ISaveObject& operator<<(u8& Value) = 0;
	virtual ISaveObject& operator<<(s8& Value) = 0;
	virtual ISaveObject& operator<<(bool& Value) = 0;
	virtual ISaveObject& operator<<(shared_str& S) = 0;
};