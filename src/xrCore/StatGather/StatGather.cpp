#include "stdafx.h"
#include "StatGather.h"

CStatGather& CStatGather::GetInstance()
{
	static CStatGather instance;
	return instance;
}

void CStatGather::AddModel(LPCSTR name)
{
	xrCriticalSectionGuard Guard(ModelsMutex);

	xr_string str_name = name;
	str_name = str_name.erase(str_name.find_last_of(':'), str_name.size() - 1);

	Models.emplace(str_name.c_str());
}

void CStatGather::AddTexture(LPCSTR name)
{
	xrCriticalSectionGuard Guard(TexturesMutex);
	Textures.emplace(name);
}

void CStatGather::AddShader(LPCSTR name)
{
	xrCriticalSectionGuard Guard(ShadersMutex);
	Shaders.emplace(name);
}

void CStatGather::AddOMF(LPCSTR name)
{
	xrCriticalSectionGuard Guard(OMFsMutex);
	OMFs.emplace(name);
}

void CStatGather::AddPE(LPCSTR name)
{
	xrCriticalSectionGuard Guard(PEsMutex);
	PEs.emplace(name);
}

void CStatGather::AddPG(LPCSTR name)
{
	xrCriticalSectionGuard Guard(PGsMutex);
	PGs.emplace(name);
}

void CStatGather::DumpStats()
{
	xrCriticalSectionGuard ModelsGuard(ModelsMutex);
	xrCriticalSectionGuard TexturesGuard(TexturesMutex);
	xrCriticalSectionGuard ShadersGuard(ShadersMutex);
	xrCriticalSectionGuard OMFsGuard(OMFsMutex);
	xrCriticalSectionGuard PEsGuard(PEsMutex);
	xrCriticalSectionGuard PGsGuard(PGsMutex);

	CInifile* File = new CInifile(StatFileName, FALSE, TRUE, TRUE);
	File->set_override_names(true);

	for (auto elem : Models) {
		File->w_string("Models", elem.c_str(), nullptr);
	}
	for (auto elem : Textures) {
		File->w_string("Textures", elem.c_str(), nullptr);
	}
	for (auto elem : Shaders) {
		File->w_string("Shaders", elem.c_str(), nullptr);
	}
	for (auto elem : PEs) {
		File->w_string("ParticleEffects", elem.c_str(), nullptr);
	}
	for (auto elem : PGs) {
		File->w_string("ParticleGroups", elem.c_str(), nullptr);
	}

	CInifile::Destroy(File);
}

CStatGather::CStatGather() {
	VERIFY(Core.ParamsData.test(ECoreParams::stat_gather));

	xr_strconcat(StatFileName, Core.ApplicationName, "-", Core.UserName, ".resstat");
	if (FS.path_exist("$logs$"))
	{
		FS.update_path(StatFileName, "$logs$", StatFileName);
	}
}
