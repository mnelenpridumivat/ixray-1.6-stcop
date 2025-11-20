///////////////////////////////////////////////////////////////
// ZudaArtifact.cpp
// ZudaArtefact - артефакт "зуда"
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ZudaArtifact.h"
#ifndef IXRAY_PHYSX
#include "../xrPhysics/PhysicsShell.h"
#endif


CZudaArtefact::CZudaArtefact(void) 
{
}

CZudaArtefact::~CZudaArtefact(void) 
{
}

void CZudaArtefact::Load(LPCSTR section) 
{
	inherited::Load(section);
}
