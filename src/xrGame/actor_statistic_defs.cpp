#include "stdafx.h"
#include "actor_statistic_defs.h"

ISaveObject& operator<<(ISaveObject& Object, SStatDetailBData& Data)
{
	BEGIN_CHUNK(Object,"SStatDetailBData")
	{
		Object << Data.key << Data.int_count << Data.int_points << Data.str_value;
	}
	return Object;
}

ISaveObject& operator<<(ISaveObject& Object, SStatSectionData& Data)
{
	BEGIN_CHUNK(Object,"SStatSectionData")
	{
		Object << Data.data << Data.key;
	}
	return Object;
}
