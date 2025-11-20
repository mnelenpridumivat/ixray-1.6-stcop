#pragma once

#ifndef IXRAY_PHYSX
#include "../xrPhysics/PhysicsShell.h"
#endif
#include "PhysX/Wrappers/Element.h"

struct	CPHCaptureBoneCallback :
	public NearestToPointCallback
{
	virtual	bool operator() ( u16 bid )		= 0;
	virtual	bool operator() ( xrPhysX::Wrappers::CElement* e )	
	{
		return (*this) ( e->m_SelfID );
	};
};
