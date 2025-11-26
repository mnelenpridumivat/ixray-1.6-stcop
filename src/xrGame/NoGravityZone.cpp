#include "StdAfx.h"
#include "NoGravityZone.h"
#ifndef IXRAY_PHYSX
#include "../xrPhysics/PhysicsShell.h"
#endif
#include "entity_alive.h"
#include "PHMovementControl.h"
//#include "PhWorld.h"
#include "CharacterPhysicsSupport.h"
//extern CPHWorld	*ph_world;
#ifndef IXRAY_PHYSX
#include "../xrPhysics/IPHWorld.h"
#endif
void CNoGravityZone::enter_Zone(SZoneObjectInfo& io)
{
	inherited::enter_Zone(io);
	switchGravity(io,false);

}
void CNoGravityZone::exit_Zone(SZoneObjectInfo& io)
{
	switchGravity(io,true);
	inherited::exit_Zone(io);
	
}
void CNoGravityZone::UpdateWorkload(u32 dt)
{
	for (SZoneObjectInfo& info : m_ObjectInfoMap)
		switchGravity(info,false);
}
void CNoGravityZone::switchGravity(SZoneObjectInfo& io, bool val)
{
	if(io.object->getDestroy()) return;
	CPhysicsShellHolder* sh= smart_cast<CPhysicsShellHolder*>(io.object);
	if(!sh)return;
	auto shell=sh->PPhysicsShell();
	if(shell&&shell->isActive())
	{
		shell->set_ApplyByGravity(val);
		if(!val&&shell->get_ApplyByGravity())
		{
			auto e=shell->get_ElementByStoreOrder(u16(Random.randI(0,shell->get_ElementsNumber())));
			if(e->isActive())
			{
				e->applyImpulseTrace(Fvector().random_point(e->getRadius()),Fvector().random_dir(),shell->getMass()*physics_world()->Gravity()*fixed_step,e->m_SelfID);
			}

		}
		//shell->SetAirResistance(0.f,0.f);
		//shell->set_DynamicScales(1.f);
		return;
	}
	if(!io.nonalive_object)
	{
		CEntityAlive* ea=smart_cast<CEntityAlive*>(io.object);
		auto mc=ea->character_physics_support()->movement();
		mc->SetApplyGravity(BOOL(val));
		mc->SetForcedPhysicsControl(!val);
		if(!val&&mc->Environment()==CPHMovementControl::peOnGround)
		{
			Fvector gn;
			mc->GroundNormal(gn);
			mc->ApplyImpulse(gn,mc->GetMass()*physics_world()->Gravity()*fixed_step);
			
		}
	}
}