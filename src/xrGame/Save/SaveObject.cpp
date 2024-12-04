#include "StdAfx.h"
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

void CSaveObject::BeginChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->BeginChunk(ChunkName));
}

void CSaveObject::FindChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->FindChunk(ChunkName));
}

void CSaveObject::EndChunk()
{
	_chunkStack.pop();
	VERIFY(!_chunkStack.empty());
}
