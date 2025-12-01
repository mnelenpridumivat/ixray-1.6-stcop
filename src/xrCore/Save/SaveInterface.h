#pragma once
#include "SaveChunkHandleInterface.h"
#include "../xrCore/xrCore.h"
#include "Concepts.h"

class shared_str;

class XRCORE_API ISaveObjectStackHandler
{
	u64 depth = u64(-1);
public:
	ISaveObjectStackHandler(u64 depth) : depth(depth){}
	u64 GetDepth()const { return depth; }
};

class XRCORE_API ISaveObject {
public:
	virtual ~ISaveObject() = default;
	virtual ISaveObjectStackHandler BeginChunk(shared_str ChunkName) = 0;
	virtual void EndChunk(ISaveObjectStackHandler handler) = 0;
	virtual void BeginArray() = 0;
	virtual void EndArray() = 0;

	virtual bool HasChunk(shared_str ChunkName) = 0;

	virtual bool IsSave() = 0;
	
	// simple data chunk extraction - if need to store ALife online data for offline object
	virtual CSaveChunk* ExtractCurrentChunkRaw() = 0;
	virtual void MergeSubchunk(CSaveChunk* Chunk) = 0;
	
	// in case if mp support is required, and save data could possibly be on client - base for replication
	virtual u64 ExtractCurrentChunk() = 0;
	virtual void MergeChunkByHandle(ISaveChunkHandleInterface* handle) = 0;
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

class XRCORE_API ISaveObjectStackGuard
{
	ISaveObjectStackHandler handler;
	ISaveObject* saveObject = nullptr;
public:
	ISaveObjectStackGuard(ISaveObject* saveObject, ISaveObjectStackHandler handler) : handler(handler),
		saveObject(saveObject) {}
	~ISaveObjectStackGuard(){ saveObject->EndChunk(handler); }
};

template<typename T>
concept IsSaveObjectSerializablePtr = requires(ISaveObject& Object, T Value)
{
	{Object << (*Value)} -> std::same_as<ISaveObject&>;
} && std::is_pointer_v<T>;

template<typename T>
concept IsSaveObjectSerializableRef = requires(ISaveObject& Object, T& Value)
{
	{Object << Value} -> std::same_as<ISaveObject&>;
};

template<typename T>
concept IsSaveObjectSerializable = IsSaveObjectSerializableRef<T> || (std::is_pointer_v<T> && IsSaveObjectSerializablePtr<T>);

template<XRay::Concepts::Enum T>
ISaveObject& operator<<(ISaveObject& Object, T& Value)
{
	std::underlying_type_t<T> Casted = (std::underlying_type_t<T>)Value;
	Object << Casted;
	Value = (T)Casted;
	return Object;
}

#define BEGIN_CHUNK(Obj, Name) if((Obj).IsSave() || (Obj).HasChunk(Name)) if(ISaveObjectStackGuard guard(&(Obj), (Obj).BeginChunk(Name)); true)
