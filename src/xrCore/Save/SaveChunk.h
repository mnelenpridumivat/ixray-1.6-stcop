#pragma once
#include "../xrScripts/script_export_space.h"
#include "SaveVariables.h"

class CMemoryBuffer;

class XRCORE_API CSaveChunk: public ISaveable {

	shared_str _chunkName;
	xr_map<shared_str, CSaveChunk*> _subchunks;
	xr_vector<CSaveVariableBase*> _variables;
	u64 _currentReadIndex;
	xr_stack<CSaveVariableArray*> _currentArrayStack;

public:
	CSaveChunk(shared_str ChunkName) : _chunkName(ChunkName) {}
	~CSaveChunk();

	void Write(CMemoryBuffer& Buffer);

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_chunk; }
	virtual bool IsArray() override { return false; }

	void ReadArray(u64& Size);
	void WriteArray(u64 Size);
	void EndArray();

	CSaveChunk* BeginChunk(shared_str ChunkName);
	CSaveChunk* FindChunk(shared_str ChunkName);

	// writing - utilities
	void w_bool(bool a);
	void w_float(float a);
	void w_double(double a);
	void w_vec3(const Fvector& a);
	void w_vec4(const Fvector4& a);
	void w_u64(u64 a);
	void w_s64(s64 a);
	void w_u32(u32 a);
	void w_s32(s32 a);
	void w_u16(u16 a);
	void w_s16(s16 a);
	void w_u8(u8 a);
	void w_s8(s8 a);
	/*void w_float_q16(float a, float min, float max);
	void w_float_q8(float a, float min, float max);
	void w_angle16(float a);
	void w_angle8(float a);
	void w_dir(const Fvector& D);
	void w_sdir(const Fvector& D);*/
	void w_stringZ(LPCSTR S);

	void w_string(LPCSTR S) { w_stringZ(S); }

	void w_stringZ(const shared_str& p);
	void w_stringZ(const xr_string& p);
	void w_matrix(const Fmatrix& M);
	void w_clientID(ClientID& C);

	// reading - utilities
	void r_bool(bool& A);
	void r_vec3(Fvector& A);
	void r_vec4(Fvector4& A);
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
	/*void r_float_q16(float& A, float min, float max);
	void r_float_q8(float& A, float min, float max);
	void r_angle16(float& A);
	void r_angle8(float& A);
	void r_dir(Fvector& A);
	void r_sdir(Fvector& A);*/
	void r_stringZ(LPSTR S);
	void r_stringZ(xr_string& dest);
	void r_stringZ(shared_str& dest);
	void r_stringZ_s(LPSTR string, u32 size);

	template <u32 size>
	inline void	r_stringZ_s(char(&string)[size])
	{
		r_stringZ_s(string, size);
	}

	void r_matrix(Fmatrix& M);
	void r_clientID(ClientID& C);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};