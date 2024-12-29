#include "stdafx.h"
#include "GameTaskDefs.h"

ISaveObject& operator<<(ISaveObject& Object, SGameTaskKey& Data)
{
    Data.serialize(Object);
    return Object;
}
