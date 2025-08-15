#pragma once
#include "SaveInterface.h"
#include "../xrScripts/script_export_space.h"
#include "SaveChunk.h"
#include "fastdelegate.h"
#include "type_traits"
#include "shared_string.h"
#include "associative_vector.h"

class XRCORE_API CSaveObject:
	public ISaveObject
{
protected:
	CSaveChunk* _rootChunk;
	xr_stack<CSaveChunk*> _chunkStack;
	bool _isPartial = false;

#ifndef MASTER_GOLD
	xr_queue<shared_str> _debugTopChunkNamesQueue;
#endif

	CSaveChunk* GetCurrentChunk();

public:
#ifndef MASTER_GOLD
	void ClearDebugData();
	void PopDebugData();
#endif
	
	CSaveObject();
	CSaveObject(CSaveChunk* Root);
	~CSaveObject();
	void EndChunk(ISaveObjectStackHandler handler) override;
	void EndArray() override;
	bool HasChunk(shared_str ChunkName) override;

	//void MarkDirty();
	virtual u64 GetChunkStackDepth() override {return _chunkStack.size();}

	template<typename Key, typename Mapped>
	ISaveObject& Serialize(xr_map<Key, Mapped>& Value) {
		if (IsSave()) {
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				BEGIN_CHUNK((*this), "MapElem")
				{
					if constexpr (std::is_pointer_v<Key>) {
						(*this) << *(elem.first);
					}
					else {
						Key Value = elem.first;
						(*this) << Value;
					}
					if constexpr (std::is_pointer_v<Mapped>) {
						(*this) << *(elem.second);
					}
					else {
						(*this) << elem.second;
					}
				}
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				BEGIN_CHUNK((*this), "MapElem")
				{
					std::pair<Key, Mapped> Elem;
					if constexpr (std::is_pointer_v<Key>) {
						(*this) << *(Elem.first);
					}
					else {
						(*this) << Elem.first;
					}
					if constexpr (std::is_pointer_v<Mapped>) {
						(*this) << *(Elem.second);
					}
					else {
						(*this) << Elem.second;
					}
					Value.insert(Elem);
				}
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename Key, typename Mapped>
	ISaveObject& Serialize(xr_map<Key, Mapped>& Value, fastdelegate::FastDelegate<void(ISaveObject&, typename std::pair<Key, Mapped>&)> PerElem) {
		if (IsSave()) {
			//GetCurrentChunk()->WriteArray(Value.size());
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				std::pair<Key, Mapped> Elem = elem;
				PerElem(*this, Elem);
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				std::pair<Key, Mapped> Elem;
				PerElem(*this, Elem);
				Value.insert(Elem);
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename T, size_t Size>
	ISaveObject& Serialize(svector<T, Size>& Value) {
		if (IsSave()) {
			GetCurrentChunk()->WriteArray();
			for (u64 i = 0; i < Size; ++i) {
				if constexpr (std::is_pointer_v<T>) {
					(*this) << *(Value[i]);
				}
				else {
					(*this) << Value[i];
				}
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				if constexpr (std::is_pointer_v<T>) {
					//CreateElem(Value);
					T Elem = new std::remove_pointer_t<T>();
					(*this) << *(Value[i]);
					Value[i] = Elem;
				}
				else {
					(*this) << Value[i];
				}
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename Key, typename Mapped>
	ISaveObject& Serialize(associative_vector<Key, Mapped>& Value) {
		if (IsSave()) {
			//GetCurrentChunk()->WriteArray(Value.size());
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				BEGIN_CHUNK((*this), "MapElem")
				{
					if constexpr (std::is_pointer_v<Key>) {
						(*this) << *(elem.first);
					}
					else {
						(*this) << elem.first;
					}
					if constexpr (std::is_pointer_v<Mapped>) {
						(*this) << *(elem.second);
					}
					else {
						(*this) << elem.second;
					}
				}
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				BEGIN_CHUNK((*this), "MapElem")
				{
					std::pair<Key, Mapped> Elem;
					if constexpr (std::is_pointer_v<Key>) {
						Elem.first = new std::remove_pointer_t<Key>();
						(*this) << *(Elem.first);
					}
					else {
						Elem.first = Key();
						(*this) << Elem.first;
					}
					if constexpr (std::is_pointer<Mapped>::value) {
						Elem.second = new std::remove_pointer_t<Mapped>();
						(*this) << *(Elem.second);
					}
					else {
						Elem.second = Mapped();
						(*this) << Elem.second;
					}
					Value.insert(Elem);
				}
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename Key, typename Mapped>
	ISaveObject& Serialize(associative_vector<Key, Mapped>& Value, fastdelegate::FastDelegate<void(ISaveObject&, typename std::pair<Key, Mapped>&)> PerElem) {
		if (IsSave()) {
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				PerElem(*this, elem);
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				std::pair<Key, Mapped> Elem;
				PerElem(*this, Elem);
				Value.insert(Elem);
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename T, size_t Size>
	ISaveObject& Serialize(T (&Value)[Size]) {
		if (IsSave()) {
			GetCurrentChunk()->WriteArray();
			for (u64 i = 0; i < Size; ++i) {
				if constexpr (std::is_pointer_v<T>) {
					(*this) << *(Value[i]);
				}
				else {
					(*this) << Value[i];
				}
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				if constexpr (std::is_pointer_v<T>) {
					T Elem = new std::remove_pointer_t<T>();
					(*this) << *(Value[i]);
					Value[i] = Elem;
				}
				else {
					(*this) << Value[i];
				}
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename T>
	ISaveObject& Serialize(xr_vector<T>& Value)
	{
		if (IsSave()) {
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				if constexpr (std::is_pointer_v<T>) {
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
				if constexpr (std::is_pointer_v<T>) {
					T Elem = new std::remove_pointer_t<T>();
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
	ISaveObject& Serialize(xr_vector<xr_shared_ptr<T>>& Value)
	{
		if (IsSave()) {
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				(*this) << *elem;
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				Value.push_back(xr_make_shared<T>());
				(*this) << *Value.back();
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename T>
	ISaveObject& Serialize(xr_vector<xr_unique_ptr<T>>& Value)
	{
		if (IsSave()) {
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				(*this) << *elem;
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				Value.push_back(xr_make_unique<T>());
				(*this) << *Value.back();
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
			//GetCurrentChunk()->WriteArray(Value.size());
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				if constexpr (std::is_pointer_v<T>) {
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
				if constexpr (std::is_pointer_v<T>) {
					auto Elem = new std::remove_pointer_t<T>();
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
	
	template<typename T, typename H, typename Eq>
	ISaveObject& Serialize(xr_hash_set<T, H, Eq>& Value)
	{
		if (IsSave())
		{
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				if constexpr (std::is_pointer_v<T>) {
					(*this) << *elem;
				}
				else {
					(*this) << elem;
				}
			}
			
		} else
		{
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				if constexpr (std::is_pointer_v<T>) {
					//CreateElem(Value);
					T Elem = new std::remove_pointer_t<T>();
					(*this) << *Elem;
					Value.emplace(Elem);
				}
				else {
					T&& Elem = T();
					(*this) << Elem;
					Value.emplace(Elem);
				}
			}
			
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename Key, typename Mapped>
	ISaveObject& Serialize(xr_hash_map<Key, Mapped>& Value) {
		if (IsSave()) {
			//GetCurrentChunk()->WriteArray(Value.size());
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				BEGIN_CHUNK((*this), "MapElem")
				{
					if constexpr (std::is_pointer_v<Key>) {
						(*this) << *(elem.first);
					}
					else {
						Key Value = elem.first;
						(*this) << Value;
					}
					if constexpr (std::is_pointer_v<Mapped>) {
						(*this) << *(elem.second);
					}
					else {
						(*this) << elem.second;
					}
				}
			}
		}
		else {
			u64 ArrSize;
			GetCurrentChunk()->ReadArray(ArrSize);
			for (u64 i = 0; i < ArrSize; ++i) {
				BEGIN_CHUNK((*this), "MapElem")
				{
					std::pair<Key, Mapped> Elem;
					if constexpr (std::is_pointer_v<Key>) {
						(*this) << *(Elem.first);
					}
					else {
						(*this) << Elem.first;
					}
					if constexpr (std::is_pointer_v<Mapped>) {
						(*this) << *(Elem.second);
					}
					else {
						(*this) << Elem.second;
					}
					Value.insert(Elem);
				}
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}

	template<typename T>
	ISaveObject& Serialize(xr_deque<T>& Value)
	{
		if (IsSave()) {
			GetCurrentChunk()->WriteArray();
			for (auto& elem : Value) {
				if constexpr (std::is_pointer_v<T>) {
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
					T Elem = new std::remove_pointer_t<T>();
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

};

/*template<typename T>
ISaveObject& operator<<(ISaveObject& Object, T* Value) {
	VERIFY(Value);
	Object << *Value;
}*/

template<typename T>
ISaveObject& operator<<(ISaveObject& Object, xr_vector<T>& Value) {
	return ((CSaveObject*)&Object)->Serialize(Value);
}

template<typename T, size_t Size>
ISaveObject& operator<<(ISaveObject& Object, T (&Value)[Size]) {
	return ((CSaveObject*)&Object)->Serialize(Value);
}

template<typename Key, typename Mapped>
ISaveObject& operator<<(ISaveObject& Object, associative_vector<Key, Mapped>& Value) {
	return ((CSaveObject*)&Object)->Serialize(Value);
}

template<typename Key, typename Mapped>
ISaveObject& operator<<(ISaveObject& Object, xr_map<Key, Mapped>& Value) {
	return ((CSaveObject*)&Object)->Serialize(Value);
}

template<typename T, size_t Size>
ISaveObject& operator<<(ISaveObject& Object, svector<T, Size>& Value) {
	return ((CSaveObject*)&Object)->Serialize(Value);
}

template<typename T, typename H, typename Eq>
ISaveObject& operator<<(ISaveObject& Object, xr_hash_set<T, H, Eq>& Value) {
	return ((CSaveObject*)&Object)->Serialize(Value);
}

template<typename K, typename V, typename H, typename Eq>
ISaveObject& operator<<(ISaveObject& Object, xr_hash_map<K, V, H, Eq>& Value) {
	return ((CSaveObject*)&Object)->Serialize(Value);
}

template<typename T>
ISaveObject& operator<<(ISaveObject& Object, xr_deque<T>& Value)
{
	return ((CSaveObject*)&Object)->Serialize(Value);
}

XRCORE_API ISaveObject& operator<<(ISaveObject& Object, char& Value);
XRCORE_API ISaveObject& operator<<(ISaveObject& Object, LPSTR& Value);

class XRCORE_API CSaveObjectSave: public CSaveObject {
public:
	CSaveObjectSave();
	CSaveObjectSave(CSaveChunk* Root);

	virtual ISaveObjectStackHandler BeginChunk(shared_str ChunkName) override;
	virtual void BeginArray() override;

	virtual bool IsSave() override { return true; }
	
	virtual u64 ExtractCurrentChunk() override;
	virtual void MergeChunkByHandle(ISaveChunkHandleInterface* handle) override;

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
	virtual ISaveObject& operator<<(shared_str& S) override;

	void Write(CMemoryBuffer* buffer, SSaveTask* Task);
};

class XRCORE_API CSaveObjectLoad: public CSaveObject {
public:
	CSaveObjectLoad();
	CSaveObjectLoad(ISaveChunkHandleInterface* Root);

	virtual ISaveObjectStackHandler BeginChunk(shared_str ChunkName) override;
	virtual void BeginArray() override;

	virtual bool IsSave() override { return false; }
	
	virtual u64 ExtractCurrentChunk() override;
	virtual void MergeChunkByHandle(ISaveChunkHandleInterface* handle) override;

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
	virtual ISaveObject& operator<<(shared_str& S) override;

	void Parse(IReader* stream);
};