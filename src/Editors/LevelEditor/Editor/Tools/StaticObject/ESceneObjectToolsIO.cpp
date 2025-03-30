#include "stdafx.h"

static const u16 OBJECT_TOOLS_VERSION  	= 0x0000;

enum{
    CHUNK_VERSION			= 0x1001ul,
    CHUNK_APPEND_RANDOM		= 0x1002ul,
    CHUNK_FLAGS				= 0x1003ul,
};
bool ESceneObjectTool::LoadLTX(CInifile& ini)
{
	u32 version 	= ini.r_u32("main", "version");
    if( version!=OBJECT_TOOLS_VERSION )
    {
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
    }

	inherited::LoadLTX		(ini);

   	m_Flags.assign			(ini.r_u32("main", "flags"));

    m_AppendRandomMinScale = ini.r_fvector3	("AppendRandom", "AppendRandomMinScale");
    m_AppendRandomMaxScale	= ini.r_fvector3	("AppendRandom", "AppendRandomMaxScale");
    m_AppendRandomMinRotation = ini.r_fvector3	("AppendRandom", "AppendRandomMinRotation");
    m_AppendRandomMaxRotation = ini.r_fvector3	("AppendRandom", "AppendRandomMaxRotation");
    u32 cnt =   ini.r_u32		("AppendRandom", "AppendRandomObjects_size");

    for (int i=0; i<cnt; ++i)
    {
        string128 			buff;
        sprintf				(buff,"object_name_%d",i);
        shared_str s		= ini.r_string("AppendRandom", buff);
        m_AppendRandomObjects.push_back	(s);
    }

    m_Flags.set(flAppendRandom,FALSE);

    return true;
}

bool ESceneObjectTool::LoadJSON(nlohmann::json& file)
{
	u32 version 	= file["main"]["version"].get<u32>();
	if( version!=OBJECT_TOOLS_VERSION )
	{
		ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
		return false;
	}

	inherited::LoadJSON		(file);

	m_Flags.assign			(file["main"]["flags"].get<u32>());

	m_AppendRandomMinScale.x = file["AppendRandom"]["AppendRandomMinScale"]["x"].get<float>();
	m_AppendRandomMinScale.y = file["AppendRandom"]["AppendRandomMinScale"]["y"].get<float>();
	m_AppendRandomMinScale.z = file["AppendRandom"]["AppendRandomMinScale"]["z"].get<float>();
	m_AppendRandomMaxScale.x = file["AppendRandom"]["AppendRandomMaxScale"]["x"].get<float>();
	m_AppendRandomMaxScale.y = file["AppendRandom"]["AppendRandomMaxScale"]["y"].get<float>();
	m_AppendRandomMaxScale.z = file["AppendRandom"]["AppendRandomMaxScale"]["z"].get<float>();
	m_AppendRandomMinRotation.x = file["AppendRandom"]["AppendRandomMinRotation"]["x"].get<float>();
	m_AppendRandomMinRotation.y = file["AppendRandom"]["AppendRandomMinRotation"]["y"].get<float>();
	m_AppendRandomMinRotation.z = file["AppendRandom"]["AppendRandomMinRotation"]["z"].get<float>();
	m_AppendRandomMaxRotation.x = file["AppendRandom"]["AppendRandomMaxRotation"]["x"].get<float>();
	m_AppendRandomMaxRotation.y = file["AppendRandom"]["AppendRandomMaxRotation"]["y"].get<float>();
	m_AppendRandomMaxRotation.z = file["AppendRandom"]["AppendRandomMaxRotation"]["z"].get<float>();
	xr_vector<std::string> temp_vec = file["AppendRandom"]["Objects"];
	m_AppendRandomObjects.clear();
	for(const auto& obj : temp_vec)
	{
		m_AppendRandomObjects.emplace_back(obj.c_str());
	}

	m_Flags.set(flAppendRandom,FALSE);

	return true;
}

void ESceneObjectTool::SaveLTX(CInifile& ini, int id)
{
	inherited::SaveLTX(ini, id);

	ini.w_u32		("main", "version", OBJECT_TOOLS_VERSION);

    ini.w_u32		("main", "flags", m_Flags.get());

    ini.w_fvector3	("AppendRandom", "AppendRandomMinScale", m_AppendRandomMinScale);
    ini.w_fvector3	("AppendRandom", "AppendRandomMaxScale", m_AppendRandomMaxScale);
    ini.w_fvector3	("AppendRandom", "AppendRandomMinRotation", m_AppendRandomMinRotation);
    ini.w_fvector3	("AppendRandom", "AppendRandomMaxRotation", m_AppendRandomMaxRotation);
    ini.w_u32		("AppendRandom", "AppendRandomObjects_size", m_AppendRandomObjects.size());

    if (m_AppendRandomObjects.size())
    {
    	u32 i=0;
    	for (RStringVecIt it=m_AppendRandomObjects.begin(); it!=m_AppendRandomObjects.end(); ++it, ++i)
        {
        	string128 			buff;
            sprintf				(buff,"object_name_%d",i);
            ini.w_string		("AppendRandom", buff, (*it).c_str());
        }
    }
}

void ESceneObjectTool::SaveJSON(nlohmann::json& file, int id)
{
	inherited::SaveJSON(file, id);

	file["main"]["version"] = OBJECT_TOOLS_VERSION;
	file["main"]["flags"] = m_Flags.get();

	file["AppendRandom"]["AppendRandomMinScale"]["x"] = m_AppendRandomMinScale.x;
	file["AppendRandom"]["AppendRandomMinScale"]["y"] = m_AppendRandomMinScale.y;
	file["AppendRandom"]["AppendRandomMinScale"]["z"] = m_AppendRandomMinScale.z;
	file["AppendRandom"]["AppendRandomMaxScale"]["x"] = m_AppendRandomMaxScale.x;
	file["AppendRandom"]["AppendRandomMaxScale"]["y"] = m_AppendRandomMaxScale.y;
	file["AppendRandom"]["AppendRandomMaxScale"]["z"] = m_AppendRandomMaxScale.z;
	file["AppendRandom"]["AppendRandomMinRotation"]["x"] = m_AppendRandomMinRotation.x;
	file["AppendRandom"]["AppendRandomMinRotation"]["y"] = m_AppendRandomMinRotation.y;
	file["AppendRandom"]["AppendRandomMinRotation"]["z"] = m_AppendRandomMinRotation.z;
	file["AppendRandom"]["AppendRandomMaxRotation"]["x"] = m_AppendRandomMaxRotation.x;
	file["AppendRandom"]["AppendRandomMaxRotation"]["y"] = m_AppendRandomMaxRotation.y;
	file["AppendRandom"]["AppendRandomMaxRotation"]["z"] = m_AppendRandomMaxRotation.z;
	xr_vector<std::string> temp_vec(0);
	temp_vec.reserve(m_AppendRandomObjects.size());
	for(const auto& elem : m_AppendRandomObjects)
	{
		temp_vec.emplace_back(elem.c_str());
	}
	file["AppendRandom"]["Objects"] = temp_vec;
}

bool ESceneObjectTool::LoadStream(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version)){
        if( version!=OBJECT_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }
    }
	if (!inherited::LoadStream(F)) return false;

    if (F.find_chunk(CHUNK_FLAGS))
    	m_Flags.assign(F.r_u32());

    if (F.find_chunk(CHUNK_APPEND_RANDOM)){
        F.r_fvector3(m_AppendRandomMinScale);
        F.r_fvector3(m_AppendRandomMaxScale);
        F.r_fvector3(m_AppendRandomMinRotation);
        F.r_fvector3(m_AppendRandomMaxRotation);
        int cnt		= F.r_u32();
        if (cnt){
        	shared_str	buf;
            for (int i=0; i<cnt; i++){
                F.r_stringZ						(buf);
                m_AppendRandomObjects.push_back	(buf);
            }
        }
    };

    m_Flags.set(flAppendRandom,FALSE);

    return true;
}


void ESceneObjectTool::SaveStream(IWriter& F)
{
	inherited::SaveStream(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&OBJECT_TOOLS_VERSION,sizeof(OBJECT_TOOLS_VERSION));

	F.open_chunk	(CHUNK_FLAGS);
    F.w_u32			(m_Flags.get());
	F.close_chunk	();

    F.open_chunk	(CHUNK_APPEND_RANDOM);
    F.w_fvector3	(m_AppendRandomMinScale);
    F.w_fvector3	(m_AppendRandomMaxScale);
    F.w_fvector3	(m_AppendRandomMinRotation);
    F.w_fvector3	(m_AppendRandomMaxRotation);
    F.w_u32			(m_AppendRandomObjects.size());
    if (m_AppendRandomObjects.size()){
    	for (RStringVecIt it=m_AppendRandomObjects.begin(); it!=m_AppendRandomObjects.end(); ++it)
            F.w_stringZ(*it);
    }
    F.close_chunk	();
}


bool ESceneObjectTool::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=OBJECT_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}


void ESceneObjectTool::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&OBJECT_TOOLS_VERSION,sizeof(OBJECT_TOOLS_VERSION));

	inherited::SaveSelection(F);
}



bool ESceneObjectTool::ExportGame(SExportStreams* F)
{
	if (!inherited::ExportGame(F)) 	return false;

    // export breakable objects
    if (!ExportBreakableObjects(F))	return false;
    if (!ExportClimableObjects(F))	return false;

	return true;
}


void ESceneObjectTool::GetStaticDesc(int& v_cnt, int& f_cnt, bool b_selected_only, bool b_cform )
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	CSceneObject* obj = (CSceneObject*)(*it);

        if(b_selected_only && !obj->Selected())
        	continue;

		if(obj->IsStatic()||(b_cform && obj->IsMUStatic()))
        {
			f_cnt	+= obj->GetFaceCount();
    	    v_cnt	+= obj->GetVertexCount();
        }
    }
}


