#pragma once
#include "SaveInterface.h"
#include "../xrScripts/script_export_space.h"
#include "SaveChunk.h"
#include "fastdelegate.h"
#include "type_traits"
#include "shared_string.h"

class XRCORE_API CSaveObject:
	public ISaveObject
{
protected:
	CSaveChunk* _rootChunk;
	xr_stack<CSaveChunk*> _chunkStack;
	bool _isPartial = false;

	CSaveChunk* GetCurrentChunk();

public:
	CSaveObject();
	CSaveObject(CSaveChunk* Root);
	~CSaveObject();
	void EndChunk() override;

	template<typename T>
	ISaveObject& operator<<(xr_vector<T>& Value) {
		Serialize(Value);
	}

	template<typename T>
	ISaveObject& Serialize(xr_vector<T>& Value)
	{
		if (IsSave()) {
			GetCurrentChunk()->WriteArray(Value.size());
			for (auto& elem : Value) {
				if constexpr (std::is_pointer<T>::value) {
					(*this) << *elem;
				}
				else {
					(*this) << elem;
				}
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				if constexpr (std::is_pointer<T>::value) {
					//CreateElem(Value);
					T Elem = new std::remove_pointer<T>::type();
					(*this) << *Elem;
					Value.emplace_back(Elem);
				}
				else {
					T&& Elem = T();
					(*this) << Elem;
					Value.emplace_back(Elem);
				}
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename T>
	ISaveObject& Serialize(xr_vector<T>& Value, fastdelegate::FastDelegate<void(ISaveObject&, typename std::remove_pointer<T>::type&)> PerElem)
	{
		VERIFY(!PerElem.empty());
		if (IsSave()) {
			GetCurrentChunk()->WriteArray(Value.size());
			for (auto& elem : Value) {
				if constexpr (std::is_pointer<T>::value) {
					PerElem(*this, *elem);
				}
				else {
					PerElem(*this, elem);
				}
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				if constexpr (std::is_pointer<T>::value) {
					auto Elem = new std::remove_pointer<T>::type();
					PerElem(*this, *Elem);
					Value.emplace_back(Elem);
				}
				else {
					T&& Elem = T();
					PerElem(*this, Elem);
					Value.emplace_back(Elem);
				}
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

};

class XRCORE_API CSaveObjectSave: public CSaveObject {
public:
	CSaveObjectSave();
	CSaveObjectSave(CSaveChunk* Root);

	virtual void BeginChunk(shared_str ChunkName) override;

	virtual bool IsSave() override { return true; }

	virtual ISaveObject& operator<<(float& Value) override;
	virtual ISaveObject& operator<<(double& Value) override;
	virtual ISaveObject& operator<<(u64& Value) override;
	virtual ISaveObject& operator<<(s64& Value) override;
	virtual ISaveObject& operator<<(u32& Value) override;
	virtual ISaveObject& operator<<(s32& Value) override;
	virtual ISaveObject& operator<<(u16& Value) override;
	virtual ISaveObject& operator<<(s16& Value) override;
	virtual ISaveObject& operator<<(u8& Value) override;
	virtual ISaveObject& operator<<(s8& Value) override;
	virtual ISaveObject& operator<<(bool& Value) override;
	virtual ISaveObject& operator<<(LPSTR S) override;

	void Write(CMemoryBuffer* buffer);
};

class XRCORE_API CSaveObjectLoad: public CSaveObject {
public:
	CSaveObjectLoad();
	CSaveObjectLoad(CSaveChunk* Root);

	virtual void BeginChunk(shared_str ChunkName) override;

	virtual bool IsSave() override { return false; }

	virtual ISaveObject& operator<<(float& Value) override;
	virtual ISaveObject& operator<<(double& Value) override;
	virtual ISaveObject& operator<<(u64& Value) override;
	virtual ISaveObject& operator<<(s64& Value) override;
	virtual ISaveObject& operator<<(u32& Value) override;
	virtual ISaveObject& operator<<(s32& Value) override;
	virtual ISaveObject& operator<<(u16& Value) override;
	virtual ISaveObject& operator<<(s16& Value) override;
	virtual ISaveObject& operator<<(u8& Value) override;
	virtual ISaveObject& operator<<(s8& Value) override;
	virtual ISaveObject& operator<<(bool& Value) override;
	virtual ISaveObject& operator<<(LPSTR S) override;

};