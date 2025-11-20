///////////////////////////////////////////////////////////////
// ThornArtifact.cpp
// ThornArtefact - артефакт колючка
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ThornArtifact.h"
#ifndef IXRAY_PHYSX
#include "../xrPhysics/PhysicsShell.h"
#endif


CThornArtefact::CThornArtefact(void) 
{
}

CThornArtefact::~CThornArtefact(void) 
{
}

void CThornArtefact::Load(LPCSTR section) 
{
	inherited::Load(section);
}
