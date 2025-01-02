#pragma once
#include "../xrScripts/script_export_space.h"
#include "SaveVariables.h"

class CMemoryBuffer;

class XRCORE_API CSaveChunk: public ISaveable {

	shared_str _chunkName;
	xr_map<shared_str, CSaveChunk*> _subchunks;
	xr_vector<CSaveVariableBase*> _variables;
	u64 _currentReadIndex;
	xr_stack<ISaveVariableArray*> _currentArrayStack;

	void ParseRec(IReader* stream, ESaveVariableType type_key);

public:
	CSaveChunk(shared_str ChunkName) : _chunkName(ChunkName) {}
	~CSaveChunk();

	void Write(CMemoryBuffer& Buffer);

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_chunk; }

	void ReadArray(u64& Size);
	void WriteArray(u64 Size);
	void EndArray();

	CSaveChunk* BeginChunk(shared_str ChunkName);
	CSaveChunk* FindChunk(shared_str ChunkName);

	// writing - utilities
	void w_bool(bool a);
	void w_float(float a);
	void w_double(double a);
	void w_u64(u64 a);
	void w_s64(s64 a);
	void w_u32(u32 a);
	void w_s32(s32 a);
	void w_u16(u16 a);
	void w_s16(s16 a);
	void w_u8(u8 a);
	void w_s8(s8 a);
	void w_string(LPCSTR S);

	// reading - utilities
	void r_bool(bool& A);
	void r_float(float& A);
	void r_double(double& A);
	void r_u64(u64& A);
	void r_s64(s64& A);
	void r_u32(u32& A);
	void r_s32(s32& A);
	void r_u16(u16& A);
	void r_s16(s16& A);
	void r_u8(u8& A);
	void r_s8(s8& A);
	void r_string(LPSTR S);

	void Parse(IReader* stream);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};