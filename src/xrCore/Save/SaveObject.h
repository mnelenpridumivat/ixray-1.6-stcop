#pragma once
#include "../xrScripts/script_export_space.h"
#include "SaveChunk.h"

class XRCORE_API CSaveObject {
protected:
	CSaveChunk* _rootChunk;
	xr_stack<CSaveChunk*> _chunkStack;
	bool _isPartial = false;

public:
	CSaveObject();
	CSaveObject(CSaveChunk* Root);
	~CSaveObject();

	CSaveChunk* GetCurrentChunk();
	virtual void BeginChunk(shared_str ChunkName) = 0;
	void EndChunk();

};

class XRCORE_API CSaveObjectSave: public CSaveObject {
public:
	CSaveObjectSave();
	CSaveObjectSave(CSaveChunk* Root);

	virtual void BeginChunk(shared_str ChunkName) override;

	void Write(CMemoryBuffer* buffer);
};

class XRCORE_API CSaveObjectLoad: public CSaveObject {
public:
	CSaveObjectLoad();
	CSaveObjectLoad(CSaveChunk* Root);

	virtual void BeginChunk(shared_str ChunkName) override;
};