#pragma once
#include "../xrScripts/script_export_space.h"
#include "SaveChunk.h"

class XRCORE_API CSaveObject {
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

class XRCORE_API CSaveObjectSave: public CSaveObject {
public:
	virtual void BeginChunk(shared_str ChunkName) override;
	virtual void FindChunk(shared_str ChunkName) override;
};

class XRCORE_API CSaveObjectLoad: public CSaveObject {
public:
	virtual void BeginChunk(shared_str ChunkName) override;
	virtual void FindChunk(shared_str ChunkName) override;
};