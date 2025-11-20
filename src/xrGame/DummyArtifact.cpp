///////////////////////////////////////////////////////////////
// DummyArtifact.cpp
// DummyArtefact - артефакт пустышка
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DummyArtifact.h"
#ifndef IXRAY_PHYSX
#include "../xrPhysics/PhysicsShell.h"
#endif


CDummyArtefact::CDummyArtefact(void) 
{
}

CDummyArtefact::~CDummyArtefact(void) 
{
}

void CDummyArtefact::Load(LPCSTR section) 
{
	inherited::Load(section);
}

