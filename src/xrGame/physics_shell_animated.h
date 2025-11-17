#pragma once
#include "PhysX/Wrappers/PhysXShell.h"

class	CPhysicsShellHolder;
class physics_shell_animated
{
protected:
	xrPhysX::Wrappers::CPhysXShell* physics_shell;
	bool update_velocity;
	
public:
	physics_shell_animated(CPhysicsShellHolder* ca, bool _update_velocity);
	virtual ~physics_shell_animated();
	
	const xrPhysX::Wrappers::CPhysXShell* shell() const {return physics_shell;}
	xrPhysX::Wrappers::CPhysXShell* shell() {return physics_shell;}
	
public:
	bool update(const Fmatrix& xrorm);
	
protected:
	virtual	void create_shell(CPhysicsShellHolder* O);
};
