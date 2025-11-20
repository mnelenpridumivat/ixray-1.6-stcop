#pragma once
#ifndef IXRAY_PHYSX
#include "../xrPhysics/iphysics_scripted.h"
#include "../xrPhysics/PhysicsShell.h"
#endif
#include "physics_shell_scripted.h"
#include "../xrScripts/script_export_space.h"
#include "PhysX/Wrappers/Element.h"

class cphysics_element_scripted:
public xrPhysX::cphysics_game_scripted<xrPhysX::Wrappers::CElement>
{
public:
	cphysics_element_scripted(xrPhysX::Wrappers::CElement* imp ):cphysics_game_scripted<xrPhysX::Wrappers::CElement>(imp){}

	void	applyForce			( float x, float y, float z )	{ physics_impl().applyForce( x, y, z ); }
	bool	isBreakable			( )								{ return physics_impl(). isBreakable( ); }
	void	get_LinearVel		( Fvector& velocity )	const	{ physics_impl().get_LinearVel( velocity ); }
	void	get_AngularVel		( Fvector& velocity )	const	{ physics_impl().get_AngularVel( velocity ); }


	float	getMass				()								{ return physics_impl().getMass	();	}
	float	getDensity			()								{ return physics_impl().getDensity(); }
	float	getVolume			()								{ return physics_impl().getVolume(); }

	void	Fix					()								{ physics_impl().Fix();	}				
	void	ReleaseFixed		()								{ physics_impl().ReleaseFixed(); }
	bool	isFixed				()								{ return physics_impl().isFixed(); }
	void	GetGlobalTransformDynamic(Fmatrix* m)		const	{ physics_impl().GetGlobalTransformDynamic( m ); }

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
