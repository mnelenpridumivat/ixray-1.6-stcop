#pragma once
#include "../xrScripts/script_export_space.h"
#include "SaveChunk.h"
#include "fastdelegate.h"
#include "type_traits"

class XRCORE_API CSaveObject {
protected:
	CSaveChunk* _rootChunk;
	xr_stack<CSaveChunk*> _chunkStack;
	bool _isPartial = false;

	CSaveChunk* GetCurrentChunk();

public:
	CSaveObject();
	CSaveObject(CSaveChunk* Root);
	~CSaveObject();
	virtual void BeginChunk(shared_str ChunkName) = 0;
	void EndChunk();

	virtual bool IsSave() = 0;

	/*virtual void s_vec3(Fvector& Value) = 0;
	virtual void s_vec4(Fvector4& a) = 0;
	virtual void s_quat(Fquaternion& Value) = 0;
	virtual void s_float(float& Value) = 0;
	virtual void s_u64(u64& Value) = 0;
	virtual void s_s64(s64& Value) = 0;
	virtual void s_u32(u32& Value) = 0;
	virtual void s_s32(s32& Value) = 0;
	virtual void s_u16(u16& Value) = 0;
	virtual void s_s16(s16& Value) = 0;
	virtual void s_u8(u8& Value) = 0;
	virtual void s_s8(s8& Value) = 0;
	virtual void s_bool(bool& Value) = 0;
	virtual void s_string(LPSTR S) = 0;*/

	virtual CSaveObject& operator<<(float& Value) = 0;
	virtual CSaveObject& operator<<(double& Value) = 0;
	virtual CSaveObject& operator<<(u64& Value) = 0;
	virtual CSaveObject& operator<<(s64& Value) = 0;
	virtual CSaveObject& operator<<(u32& Value) = 0;
	virtual CSaveObject& operator<<(s32& Value) = 0;
	virtual CSaveObject& operator<<(u16& Value) = 0;
	virtual CSaveObject& operator<<(s16& Value) = 0;
	virtual CSaveObject& operator<<(u8& Value) = 0;
	virtual CSaveObject& operator<<(s8& Value) = 0;
	virtual CSaveObject& operator<<(bool& Value) = 0;
	virtual CSaveObject& operator<<(LPSTR S) = 0;

	template<typename T>
	CSaveObject& operator<<(xr_vector<T>& Value) {
		Serialize(Value);
	}

	template<typename T>
	CSaveObject& Serialize(xr_vector<T>& Value)
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
	CSaveObject& Serialize(xr_vector<T>& Value, fastdelegate::FastDelegate<void(CSaveObject&, typename std::remove_pointer<T>::type&)> PerElem)
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
				//CreateElem<T>(Value, PerElem);
				//Value.emplace_back(Elem);
			}
		}
		GetCurrentChunk()->EndArray();
		return *this;
	}
	//CSaveObject& Serialize(xr_vector<T> Value, void(*PerElem)(T&) = nullptr)
	//CSaveObject& Serialize(xr_vector<T> Value, fastdelegate::FastDelegate<void(T&)> PerElem)

private:
	template<typename Type>
	void CreateElem(xr_vector<Type>& Value) {
		VERIFY(IsSave());
		Type Elem = Type();
		(*this) << Elem;
		Value.emplace_back(Elem);
	}

	template<typename Type>
	void CreateElem(xr_vector<Type*>& Value) {
		VERIFY(IsSave());
		Type* Elem = new Type();
		(*this) << Elem;
		Value.emplace_back(Elem);
	}

	/*template<typename T>
	void CreateElem(xr_vector<T>& Value, void(*PerElem)(T&)) {
		VERIFY(IsSave());
		T Elem = T();
		PerElem(*this, Elem);
		Value.emplace_back(Elem);
	}

	template<typename T>
	void CreateElem(xr_vector<T*>& Value, void(*PerElem)(T&)) {
		VERIFY(IsSave());
		T* Elem = new T();
		PerElem(*this, *Elem);
		Value.emplace_back(Elem);
	}*/

	/*void s_stringZ(LPSTR S) { s_string(S); };
	virtual void s_stringZ(shared_str& p) = 0;
	virtual void s_stringZ(xr_string& p) = 0;
	virtual void s_matrix(Fmatrix& M) = 0;
	virtual void s_clientID(ClientID& C) = 0;
	virtual void s_stringZ_s(LPSTR string, u32 size) = 0;

	template <u32 size>
	inline void	s_stringZ_s(char(&string)[size])
	{
		r_stringZ_s(string, size);
	}*/

};

class XRCORE_API CSaveObjectSave: public CSaveObject {
public:
	CSaveObjectSave();
	CSaveObjectSave(CSaveChunk* Root);

	virtual void BeginChunk(shared_str ChunkName) override;

	virtual bool IsSave() override { return true; }

	/*virtual void s_vec3(Fvector& Value) override;
	virtual void s_vec4(Fvector4& a) override;
	virtual void s_quat(Fquaternion& Value) override;
	virtual void s_float(float& Value) override;
	virtual void s_u64(u64& Value) override;
	virtual void s_s64(s64& Value) override;
	virtual void s_u32(u32& Value) override;
	virtual void s_s32(s32& Value) override;
	virtual void s_u16(u16& Value) override;
	virtual void s_s16(s16& Value) override;
	virtual void s_u8(u8& Value) override;
	virtual void s_s8(s8& Value) override;
	virtual void s_bool(bool& Value) override;
	virtual void s_string(LPSTR S) override;

	virtual void s_stringZ(shared_str& p) override;
	virtual void s_stringZ(xr_string& p) override;
	virtual void s_matrix(Fmatrix& M) override;
	virtual void s_clientID(ClientID& C) override;
	virtual void s_stringZ_s(LPSTR string, u32 size) override;*/

	virtual CSaveObject& operator<<(float& Value) override;
	virtual CSaveObject& operator<<(double& Value) override;
	virtual CSaveObject& operator<<(u64& Value) override;
	virtual CSaveObject& operator<<(s64& Value) override;
	virtual CSaveObject& operator<<(u32& Value) override;
	virtual CSaveObject& operator<<(s32& Value) override;
	virtual CSaveObject& operator<<(u16& Value) override;
	virtual CSaveObject& operator<<(s16& Value) override;
	virtual CSaveObject& operator<<(u8& Value) override;
	virtual CSaveObject& operator<<(s8& Value) override;
	virtual CSaveObject& operator<<(bool& Value) override;
	virtual CSaveObject& operator<<(LPSTR S) override;

	void Write(CMemoryBuffer* buffer);
};

class XRCORE_API CSaveObjectLoad: public CSaveObject {
public:
	CSaveObjectLoad();
	CSaveObjectLoad(CSaveChunk* Root);

	virtual void BeginChunk(shared_str ChunkName) override;

	virtual bool IsSave() override { return false; }

	/*virtual void s_vec3(Fvector& Value) override;
	virtual void s_vec4(Fvector4& a) override;
	virtual void s_quat(Fquaternion& Value) override;
	virtual void s_float(float& Value) override;
	virtual void s_u64(u64& Value) override;
	virtual void s_s64(s64& Value) override;
	virtual void s_u32(u32& Value) override;
	virtual void s_s32(s32& Value) override;
	virtual void s_u16(u16& Value) override;
	virtual void s_s16(s16& Value) override;
	virtual void s_u8(u8& Value) override;
	virtual void s_s8(s8& Value) override;
	virtual void s_bool(bool& Value) override;
	virtual void s_string(LPSTR S) override;

	virtual void s_stringZ(shared_str& p) override;
	virtual void s_stringZ(xr_string& p) override;
	virtual void s_matrix(Fmatrix& M) override;
	virtual void s_clientID(ClientID& C) override;
	virtual void s_stringZ_s(LPSTR string, u32 size) override;*/

	virtual CSaveObject& operator<<(float& Value) override;
	virtual CSaveObject& operator<<(double& Value) override;
	virtual CSaveObject& operator<<(u64& Value) override;
	virtual CSaveObject& operator<<(s64& Value) override;
	virtual CSaveObject& operator<<(u32& Value) override;
	virtual CSaveObject& operator<<(s32& Value) override;
	virtual CSaveObject& operator<<(u16& Value) override;
	virtual CSaveObject& operator<<(s16& Value) override;
	virtual CSaveObject& operator<<(u8& Value) override;
	virtual CSaveObject& operator<<(s8& Value) override;
	virtual CSaveObject& operator<<(bool& Value) override;
	virtual CSaveObject& operator<<(LPSTR S) override;

};

template<typename T> CSaveObject& operator<<(CSaveObject& Object, _flags<T>& Value) {
	return Object << Value.flags;
}

template<size_t T> CSaveObject& operator<<(CSaveObject& Object, _flags_big<T>& Value) {
	if (!Object.IsSave()) {
		Value.zero();
	}
	for (u64 i = 0; i < _flags_big<T>::ArrSize; ++i) {
		if (Object.IsSave()) {
			auto ChunkNumber = Value.GetRawChunkData(i);
			Object << ChunkNumber;
		}
		else {
			Flags64 temp;
			Object << temp;
			Value.zero();
			Value.bor(temp.flags, i);
		}
	}
	return Object;
}

/*namespace xr_vector_save {
	template<typename T>
	void CreateElem(CSaveObject& Object, xr_vector<T>& Value) {
		VERIFY(Object.IsSave());
		T&& Elem = T();
		Elem << Object;
		Value.emplace_back(Elem);
	}

	template<typename T>
	void CreateElem(CSaveObject& Object, xr_vector<T*>& Value) {
		VERIFY(Object.IsSave());
		T* Elem = new T();
		(*Elem) << Object;
		Value.emplace_back(Elem);
	}
};

template<typename T>
CSaveObject& operator<<(CSaveObject& Object, xr_vector<T>& Value) {
	if (Object.IsSave()) {
		Object.GetCurrentChunk()->WriteArray(Value.size());
		for (auto& elem : Value) {
			Object << elem;
		}
	}
	else {
		u64 ArraySize;
		Object.GetCurrentChunk()->ReadArray(ArraySize);
		for (u64 i = 0; i < ArraySize; ++i) {
			xr_vector_save::CreateElem(Object, Value);
		}
	}
	Object.GetCurrentChunk()->EndArray();
	return Object;
}*/

