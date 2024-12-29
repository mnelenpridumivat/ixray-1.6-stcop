#include "stdafx.h"
#include "encyclopedia_article_defs.h"

ISaveObject& operator<<(ISaveObject& Object, ARTICLE_DATA& Data)
{
	Object.BeginChunk("ARTICLE_DATA");
	{
		Object << Data.receive_time << Data.article_id << Data.readed;
		{
			u8* type = (u8*)&Data.article_type;
			Object << *type;
		}
	}
	Object.EndChunk();
	return Object;
}
