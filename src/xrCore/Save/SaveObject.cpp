#include "stdafx.h"
#include "SaveObject.h"

CSaveObject::CSaveObject()
{
	_rootChunk = new CSaveChunk("Root");
	_chunkStack.push(_rootChunk);
}

CSaveObject::CSaveObject(CSaveChunk* Root)
{
	_rootChunk = Root;
	_isPartial = true;
	_chunkStack.push(_rootChunk);
}

CSaveObject::~CSaveObject()
{
	if (!_isPartial) {
		xr_delete(_rootChunk);
	}
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

CSaveObjectSave::CSaveObjectSave()
{
	//_rootChunk = new CSaveChunk("Root");
	//_chunkStack.push(_rootChunk);
}

CSaveObjectSave::CSaveObjectSave(CSaveChunk* Root)
{
	//_rootChunk = Root;
	//_isPartial = true;
	//_chunkStack.push(_rootChunk);
}

void CSaveObjectSave::BeginChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->BeginChunk(ChunkName));
}

CSaveObjectLoad::CSaveObjectLoad()
{
	_rootChunk = new CSaveChunk("Root");
	_chunkStack.push(_rootChunk);
}

CSaveObjectLoad::CSaveObjectLoad(CSaveChunk* Root)
{
	_rootChunk = Root;
	_isPartial = true;
	_chunkStack.push(_rootChunk);
}

void CSaveObjectLoad::BeginChunk(shared_str ChunkName)
{
	VERIFY(!_chunkStack.empty());
	_chunkStack.push(_chunkStack.top()->FindChunk(ChunkName));
}
