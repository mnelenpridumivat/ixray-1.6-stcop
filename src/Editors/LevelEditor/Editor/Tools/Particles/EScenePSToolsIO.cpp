#include "stdafx.h"


// chunks
static const u16 PS_TOOLS_VERSION  	= 0x0000;

enum{
    CHUNK_VERSION			= 0x1001ul,
};

bool EScenePSTool::LoadLTX(CInifile& ini)
{
	u32 version 	= ini.r_u32("main","version");
    if( version!=PS_TOOLS_VERSION )
    {
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
    }

	inherited::LoadLTX(ini);
	return true;
}

bool EScenePSTool::LoadJSON(nlohmann::json& file)
{
	u32 version 	= file["main"]["version"];
	if( version!=PS_TOOLS_VERSION )
	{
		ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
		return false;
	}

	inherited::LoadJSON(file);
	return true;
}

void EScenePSTool::SaveLTX(CInifile& ini, int id)
{
	inherited::SaveLTX	(ini, id);
	ini.w_u32		("main", "version", PS_TOOLS_VERSION);
}

void EScenePSTool::SaveJSON(nlohmann::json& file, int id)
{
	inherited::SaveJSON	(file, id);
	file["main"]["version"] = PS_TOOLS_VERSION;
}

bool EScenePSTool::LoadStream(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=PS_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }

	if (!inherited::LoadStream(F)) return false;

    return true;
}


void EScenePSTool::SaveStream(IWriter& F)
{
	inherited::SaveStream	(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&PS_TOOLS_VERSION,sizeof(PS_TOOLS_VERSION));
}


bool EScenePSTool::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=PS_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}


void EScenePSTool::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&PS_TOOLS_VERSION,sizeof(PS_TOOLS_VERSION));

	inherited::SaveSelection(F);
}


