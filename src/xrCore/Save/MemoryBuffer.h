#pragma once

class CMemoryBuffer;

template<typename T>
class CReservedMemory
{
	friend class CMemoryChunk;
	friend class CMemoryBuffer;
	void* Ptr;

	CReservedMemory(void* Location)
	{
		Ptr = Location;
	}

public:
	void Write(T Value) 
	{
		memcpy(Ptr, &Value, sizeof(T));
	}
	size_t GetDeltaFromBuffer(CMemoryBuffer* Buffer) {

	}
};

class CMemoryChunk
{
public:
	static constexpr size_t ChunkSize = 16 * 1024;

private:
	BYTE	data[ChunkSize];
	size_t		count;

public:

	template<typename T>
	CReservedMemory<T>* Pos() {
		if (!CanWrite(sizeof(T))) {
			return nullptr;
		}
		return new CReservedMemory<T>(((BYTE*)&data) + count);
	}

	bool CanWrite(size_t size);
	bool Write(const void* data, size_t size);

	template<typename T>
	bool Write(T* data) {
		return Write(data, sizeof(T));
	}

};

class CMemoryBuffer 
{
	xr_vector<CMemoryChunk*> Chunks;

	bool Write(const void* data, size_t size);

public:
	CMemoryBuffer();
	~CMemoryBuffer();

	template<typename T>
	CReservedMemory<T>* GetCurrentPosHandle() {
		auto CurrentPos = Chunks.back()->Pos<T>();
		if (!CurrentPos) {
			Chunks.push_back(new CMemoryChunk());
			CurrentPos = Chunks.back()->Pos<T>();
		}
		VERIFY(CurrentPos);
		return CurrentPos;
	}

	template<typename T>
	bool Write(T data) {
		return Write(&data, sizeof(T));
	}

	template<>
	bool Write(shared_str data) {
		Write(data.size());
		Write(data.c_str(), data.size());
		return true;
	}
};