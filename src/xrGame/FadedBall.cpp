///////////////////////////////////////////////////////////////
// FadedBall.cpp
// FadedBall - артефакт блеклый шар
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FadedBall.h"
#ifndef IXRAY_PHYSX
#include "../xrPhysics/PhysicsShell.h"
#endif


CFadedBall::CFadedBall(void) 
{
}

CFadedBall::~CFadedBall(void) 
{
}

void CFadedBall::Load(LPCSTR section) 
{
	inherited::Load(section);
}

