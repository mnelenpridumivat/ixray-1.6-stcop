#pragma once

class CSaveChunk;

class XRCORE_API ISaveChunkHandleInterface {
public:
	virtual CSaveChunk* GetChunk() = 0;
};