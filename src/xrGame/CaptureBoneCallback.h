#pragma once

#ifndef IXRAY_PHYSX
#include "../xrPhysics/PhysicsShell.h"
#endif
#include "PhysX/Wrappers/PhysXElement.h"

struct	CPHCaptureBoneCallback :
	public NearestToPointCallback
{
	virtual	bool operator() ( u16 bid )		= 0;
	virtual	bool operator() ( xrPhysX::Wrappers::CPhysXElement* e )	
	{
		return (*this) ( e->m_SelfID );
	};
};
