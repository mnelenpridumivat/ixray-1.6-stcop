#include "stdafx.h"
#include "relation_registry_defs.h"

ISaveObject& operator<<(ISaveObject& Object, SRelation& Data)
{
    Object << Data.m_iGoodwill;
    return Object;
}

ISaveObject& operator<<(ISaveObject& Object, RELATION_DATA& Data)
{
    Data.serialize(Object);
    return Object;
}
