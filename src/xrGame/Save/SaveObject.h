#pragma once
#include "../xrScripts/script_export_space.h"
#include "SaveChunk.h"

class CSaveObject {
	xr_unique_ptr<CSaveChunk> _rootChunk;
	xr_stack<CSaveChunk*> _chunkStack;

public:
	CSaveObject();

	CSaveChunk* GetCurrentChunk();
	void BeginChunk(shared_str ChunkName);
	void FindChunk(shared_str ChunkName);
	void EndChunk();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};