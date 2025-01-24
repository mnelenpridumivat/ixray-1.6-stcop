#pragma once
#include "SaveChunkHandleInterface.h"
#include "SaveChunk.h"
#include "SaveInterface.h"

class XRCORE_API CSaveChunkHandle: public ISaveChunkHandleInterface {

	CSaveChunk* HandledChunk = nullptr;

public:

	CSaveChunkHandle(ISaveObject* Parent, CSaveChunk* handled) : HandledChunk(handled) {
	};

	~CSaveChunkHandle();

	virtual CSaveChunk* GetChunk() override { return HandledChunk; };
};