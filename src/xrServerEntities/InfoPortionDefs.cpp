#include "stdafx.h"
#include "InfoPortionDefs.h"

ISaveObject& operator<<(ISaveObject& Object, INFO_DATA& Data)
{
	BEGIN_CHUNK(Object,"INFO_DATA")
	{
		Object << Data.info_id << Data.receive_time;
	}
    return Object;
}
