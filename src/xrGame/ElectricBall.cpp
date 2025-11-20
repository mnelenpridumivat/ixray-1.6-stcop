///////////////////////////////////////////////////////////////
// ElectricBall.cpp
// ElectricBall - артефакт электрический шар
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ElectricBall.h"
#ifndef IXRAY_PHYSX
#include "../xrPhysics/PhysicsShell.h"
#endif


CElectricBall::CElectricBall(void) 
{
}

CElectricBall::~CElectricBall(void) 
{
}

void CElectricBall::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CElectricBall::UpdateCLChild	()
{
	inherited::UpdateCLChild();

	if(H_Parent()) XFORM().set(H_Parent()->XFORM());
};