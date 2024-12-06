#include "stdafx.h"
#include "SaveObject.h"

CSaveObject::CSaveObject()
{
	_rootChunk = xr_make_unique<CSaveChunk>("Root");
	_chunkStack.push(_rootChunk.get());
}

CSaveChunk* CSaveObject::GetCurrentChunk()
{
	VERIFY(!_chunkStack.empty());
	return _chunkStack.top();
}

/*void CSaveObject::BeginChunk(shared_str ChunkName)
{
	VERIFY2(false, "Do not use base class CSaveObject directly!");
}

void CSaveObject::FindChunk(shared_str ChunkName)
{
	VERIFY2(false, "Do not use base class CSaveObject directly!");
}*/

void CSaveObject::EndChunk()
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.pop();
}

void CSaveObjectSave::BeginChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->BeginChunk(ChunkName));
}

void CSaveObjectSave::FindChunk(shared_str ChunkName)
{
	VERIFY2(false, "Attempt to use FindChunk while saving!");
}

void CSaveObjectLoad::BeginChunk(shared_str ChunkName)
{
	VERIFY2(false, "Attempt to use BeginChunk while loading!");
}

void CSaveObjectLoad::FindChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->FindChunk(ChunkName));
}
