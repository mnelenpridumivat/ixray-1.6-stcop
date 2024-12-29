#include "stdafx.h"
#include "actor_statistic_defs.h"

ISaveObject& operator<<(ISaveObject& Object, SStatDetailBData& Data)
{
	Object.BeginChunk("SStatDetailBData");
	{
		Object << Data.key << Data.int_count << Data.int_points << Data.str_value;
	}
	Object.EndChunk();
	return Object;
}

ISaveObject& operator<<(ISaveObject& Object, SStatSectionData& Data)
{
	Object.BeginChunk("SStatSectionData");
	{
		Object << Data.data << Data.key;
	}
	Object.EndChunk();
	return Object;
}
