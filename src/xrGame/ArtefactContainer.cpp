////////////////////////////////////////////////////////////////////////////
//	Module 		: ArtefactContainer.cpp
//	Created 	: 08.05.2023
//  Modified 	: 19.09.2023
//	Author		: Dance Maniac (M.F.S. Team)
//	Description : Artefact container
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArtefactContainer.h"
#include "Artefact.h"
#include "level.h"
#include "Actor.h"

//float af_from_container_charge_level = 1.0f;
//int af_from_container_rank = 1;
//CArtefactContainer* m_LastAfContainer = nullptr;

CArtefactContainer::CArtefactContainer()
{

}

CArtefactContainer::~CArtefactContainer()
{
}

void CArtefactContainer::Load(LPCSTR section)
{
	inherited::Load(section);
    CArtContainer::Load(section);
}

BOOL CArtefactContainer::net_Spawn(CSE_Abstract* DC)
{
    return		(inherited::net_Spawn(DC));
}

void CArtefactContainer::save(NET_Packet& packet)
{
	inherited::save(packet);
    CArtContainer::save(packet);
}

void CArtefactContainer::load(IReader& packet)
{
	inherited::load(packet);
    CArtContainer::load(packet);
}

u32 CArtefactContainer::Cost() const
{
    u32 res = CInventoryItem::Cost();
    res += CArtContainer::Cost();

    return res;
}

float CArtefactContainer::Weight() const
{
    float res = CInventoryItemObject::Weight();
    res = CArtContainer::Weight();

    return res;
}