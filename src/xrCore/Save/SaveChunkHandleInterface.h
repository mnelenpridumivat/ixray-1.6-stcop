#pragma once

class CSaveChunk;

class XRCORE_API ISaveChunkHandleInterface {
public:
	virtual ~ISaveChunkHandleInterface() = default;
	virtual CSaveChunk* GetChunk() = 0;
};