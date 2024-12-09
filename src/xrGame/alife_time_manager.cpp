////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_time_manager.cpp
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALfie time manager class
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "alife_time_manager.h"
#include "../xrEngine/date_time.h"

CALifeTimeManager::CALifeTimeManager	(LPCSTR section)
{
	init						(section);
}

CALifeTimeManager::~CALifeTimeManager	()
{
}

void CALifeTimeManager::init			(LPCSTR section)
{
	u32							years,months,days,hours,minutes,seconds;
	sscanf						(pSettings->r_string(section,"start_time"),"%d:%d:%d",&hours,&minutes,&seconds);
	sscanf						(pSettings->r_string(section,"start_date"),"%d.%d.%d",&days,&months,&years);
	m_start_game_time			= generate_time(years,months,days,hours,minutes,seconds);
	m_time_factor				= pSettings->r_float(section,"time_factor");
	m_normal_time_factor		= pSettings->r_float(section,"normal_time_factor");
	m_game_time					= m_start_game_time;
	m_start_time				= Device.dwTimeGlobal;
}

void CALifeTimeManager::save			(IWriter	&memory_stream)
{
	m_game_time					= game_time();
	m_start_time				= Device.dwTimeGlobal;
	memory_stream.open_chunk	(GAME_TIME_CHUNK_DATA);
	memory_stream.w				(&m_game_time,		sizeof(m_game_time));
	memory_stream.w_float		(m_time_factor);
	memory_stream.w_float		(m_normal_time_factor);
	memory_stream.close_chunk	();
}

void CALifeTimeManager::load			(IReader	&file_stream)
{
	R_ASSERT2					(file_stream.find_chunk(GAME_TIME_CHUNK_DATA),"Can't find chunk GAME_TIME_CHUNK_DATA!");
	file_stream.r				(&m_game_time,		sizeof(m_game_time));
	m_time_factor				= file_stream.r_float();
	m_normal_time_factor		= file_stream.r_float();
	m_start_time				= Device.dwTimeGlobal;
}

void CALifeTimeManager::Save(CSaveObjectSave* Object)
{
	Object->BeginChunk("CALifeTimeManager");
	{

		m_game_time = game_time();
		m_start_time = Device.dwTimeGlobal;
		Object->GetCurrentChunk()->w_u64(m_game_time);
		Object->GetCurrentChunk()->w_float(m_time_factor);
		Object->GetCurrentChunk()->w_float(m_normal_time_factor);
	}
	Object->EndChunk();
}

void CALifeTimeManager::Load(CSaveObjectLoad* Object)
{
	Object->BeginChunk("CALifeTimeManager");
	{
		Object->GetCurrentChunk()->r_u64(m_game_time);
		Object->GetCurrentChunk()->r_float(m_time_factor);
		Object->GetCurrentChunk()->r_float(m_normal_time_factor);
		m_start_time = Device.dwTimeGlobal;
	}
	Object->EndChunk();
}
