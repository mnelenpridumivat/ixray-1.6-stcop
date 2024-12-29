///////////////////////////////////////////////////////////////
// game_news.cpp
// реестр новостей: новости симул€ции + сюжетные
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "game_news.h"
#include "object_broker.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "game_graph.h"

#include "../xrEngine/date_time.h"
#include "xrServer_Objects_ALife_Monsters.h"

#include "specific_character.h"

GAME_NEWS_DATA::GAME_NEWS_DATA()
{
	m_type			= eNews;
//	tex_rect.set	(0.0f,0.0f,0.0f,0.0f);
	show_time		= DEFAULT_NEWS_SHOW_TIME;
}

void GAME_NEWS_DATA::save (IWriter& stream)
{
	save_data(m_type,		stream);
	save_data(news_caption,	stream);
	save_data(news_text,	stream);
	save_data(receive_time, stream);
	save_data(texture_name,	stream);
//	save_data(tex_rect,		stream);
}

/*void GAME_NEWS_DATA::load(CSaveObjectLoad* Object)
{
	Object->BeginChunk("GAME_NEWS_DATA");
	{
		{
			u8 type;
			Object->GetCurrentChunk()->r_u8(type);
			m_type = (eNewsType)type;
		}
		Object->GetCurrentChunk()->r_stringZ(news_caption);
		Object->GetCurrentChunk()->r_stringZ(news_text);
		Object->GetCurrentChunk()->r_u64(receive_time);
		Object->GetCurrentChunk()->r_stringZ(texture_name);
	}
	Object->EndChunk();
}

void GAME_NEWS_DATA::save(CSaveObjectSave* Object) const
{
	Object->BeginChunk("GAME_NEWS_DATA");
	{
		Object->GetCurrentChunk()->w_u8(m_type);
		Object->GetCurrentChunk()->w_stringZ(news_caption);
		Object->GetCurrentChunk()->w_stringZ(news_text);
		Object->GetCurrentChunk()->w_u64(receive_time);
		Object->GetCurrentChunk()->w_stringZ(texture_name);
	}
	Object->EndChunk();
}*/

ISaveObject& operator<<(ISaveObject& Object, GAME_NEWS_DATA& Data) 
{
	Object.BeginChunk("GAME_NEWS_DATA");
	{
		u8* Value = (u8*)&Data.m_type;
		Object << *Value << Data.news_caption << Data.news_text << Data.receive_time << Data.texture_name;
	}
	Object.EndChunk();
	return Object;
}

void GAME_NEWS_DATA::load (IReader& stream)
{
	load_data(m_type,		stream);
	load_data(news_caption,	stream);
	load_data(news_text,	stream);
	load_data(receive_time, stream);
	load_data(texture_name,	stream);
//	load_data(tex_rect,		stream);
}
/*
LPCSTR GAME_NEWS_DATA::SingleLineText()
{
	if( xr_strlen(full_news_text.c_str()) )
		return full_news_text.c_str();
	string128	time = "";

	// Calc current time
	u32 years, months, days, hours, minutes, seconds, milliseconds;
	split_time		(receive_time, years, months, days, hours, minutes, seconds, milliseconds);
//#pragma todo("Satan->Satan : insert carry-over")
	//xr_sprintf(time, "%02i:%02i \\n", hours, minutes);
	xr_sprintf		(time, "%02i:%02i, ", hours, minutes);
//	strconcat	(result, locationName, time, newsPhrase);

	full_news_text			= time;
//	full_news_text			+= "%c[255,189,189,224] ";
	full_news_text			+= news_caption.c_str();
	full_news_text			+= " ";
//	full_news_text			+= " %c[default]";
	full_news_text			+= news_text.c_str();

	return full_news_text.c_str();
}
*/
