#pragma once
#include "../xrScripts/script_export_space.h"
#include "SaveChunk.h"

class CSaveObject {
protected:
	xr_unique_ptr<CSaveChunk> _rootChunk;
	xr_stack<CSaveChunk*> _chunkStack;

public:
	CSaveObject();

	CSaveChunk* GetCurrentChunk();
	virtual void BeginChunk(shared_str ChunkName) = 0;
	virtual void FindChunk(shared_str ChunkName) = 0;
	void EndChunk();
};

class CSaveObjectSave: public CSaveObject {
public:
	virtual void BeginChunk(shared_str ChunkName) override;
	virtual void FindChunk(shared_str ChunkName) override;
};

class CSaveObjectLoad: public CSaveObject {
public:
	virtual void BeginChunk(shared_str ChunkName) override;
	virtual void FindChunk(shared_str ChunkName) override;
};