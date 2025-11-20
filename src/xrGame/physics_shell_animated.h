#pragma once
#include "PhysX/Wrappers/Shell.h"

class	CPhysicsShellHolder;
class physics_shell_animated
{
protected:
	xrPhysX::Wrappers::CShell* physics_shell;
	bool update_velocity;
	
public:
	physics_shell_animated(CPhysicsShellHolder* ca, bool _update_velocity);
	virtual ~physics_shell_animated();
	
	const xrPhysX::Wrappers::CShell* shell() const {return physics_shell;}
	xrPhysX::Wrappers::CShell* shell() {return physics_shell;}
	
public:
	bool update(const Fmatrix& xrorm);
	
protected:
	virtual	void create_shell(CPhysicsShellHolder* O);
};
