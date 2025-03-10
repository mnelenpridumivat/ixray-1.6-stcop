#pragma once

class XRCORE_API CStatGather {

	string_path StatFileName;

	xr_hash_set<shared_str> Models = {};
	xrCriticalSection ModelsMutex;

	xr_hash_set<shared_str> Textures = {};
	xrCriticalSection TexturesMutex;

	xr_hash_set<shared_str> Shaders = {};
	xrCriticalSection ShadersMutex;

	xr_hash_set<shared_str> OMFs = {};
	xrCriticalSection OMFsMutex;

	xr_hash_set<shared_str> PEs = {};
	xrCriticalSection PEsMutex;

	xr_hash_set<shared_str> PGs = {};
	xrCriticalSection PGsMutex;

	CStatGather();
	//~CStatGather() { DumpStats(); }

public:
	static CStatGather& GetInstance();

	CStatGather(const CStatGather& other) = delete;
	CStatGather(CStatGather&& other) = delete;
	CStatGather& operator=(const CStatGather& other) = delete;
	CStatGather& operator=(CStatGather&& other) = delete;

	void AddModel(LPCSTR name);
	void AddTexture(LPCSTR name);
	void AddShader(LPCSTR name);
	void AddOMF(LPCSTR name);
	void AddPE(LPCSTR name);
	void AddPG(LPCSTR name);

	void DumpStats();

};