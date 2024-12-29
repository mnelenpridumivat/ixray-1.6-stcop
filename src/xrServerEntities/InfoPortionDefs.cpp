#include "stdafx.h"
#include "InfoPortionDefs.h"

ISaveObject& operator<<(ISaveObject& Object, INFO_DATA& Data)
{
	Object.BeginChunk("INFO_DATA");
	{
		Object << Data.info_id << Data.receive_time;
	}
	Object.EndChunk();
    return Object;
}
