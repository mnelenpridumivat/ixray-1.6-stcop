#pragma once

#ifndef IXRAY_PHYSX
#include "../../../xrPhysics/IPhysicsShellHolder.h"
#endif
#include "PhysX/PhysXCore.h"
#include "PhysX/Interfaces/CollisionDamageReceiver.h"
#include "PhysX/Interfaces/CollisionForm.h"
#include "PhysX/Interfaces/CollisionHitCallback.h"
#include "PhysX/Interfaces/DamageSource.h"
#include "PhysX/Interfaces/PHCapture.h"
#include "PhysX/Interfaces/PHSoundPlayer.h"
#include "PhysX/Wrappers/Articulation.h"
#include "PhysX/Wrappers/Shell.h"

class ECORE_API CPhysicsShellHolderEditorBase: public xrPhysX::Interfaces::IShellHolder
{
public:
	void CreatePhysicsShell(Fmatrix* obj_xform);
    void DeletePhysicsShell();
    void UpdateObjectXform(Fmatrix &obj_xform);
    void ApplyDragForce(const Fvector &force);
protected:
	CPhysicsShellHolderEditorBase(): m_object_xform(Fidentity){}
	~CPhysicsShellHolderEditorBase() { /*DeletePhysicsShell	();*/ }
protected:
	//xrPhysX::Wrappers::CShell* m_physics_shell = nullptr;
	xrPhysX::Wrappers::CArticulation* m_articulation = nullptr;
    Fmatrix m_object_xform;
private:
  	virtual	LPCSTR ObjectName() const { return "EditorActor"; }
	virtual	LPCSTR ObjectNameVisual() const { return "unknown"; }
	virtual	LPCSTR ObjectNameSect() const { return "unknown"; }
	virtual	bool ObjectGetDestroy() const { return false; };
	virtual xrPhysX::Interfaces::ICollisionHitCallback* ObjectGetCollisionHitCallback() { return nullptr;}
	virtual	u16 ObjectID() const { return u16(-1);}
	virtual xrPhysX::Interfaces::ICollisionForm* ObjectCollisionModel() { VERIFY(false);return nullptr; }
//	virtual	IRenderVisual*				_BCL	ObjectVisual						()				 { return m_pVisual;}
	virtual xrPhysX::Interfaces::IDamageSource* ObjectCastIDamageSource() { return nullptr; }
	virtual	void ObjectProcessingDeactivate() {}
	virtual	void ObjectProcessingActivate() {}
	virtual	void ObjectSpatialMove() {}
    //virtual xrPhysX::Wrappers::CShell* ObjectPPhysicsShell() { return m_physics_shell; }
	virtual	void enable_notificate() {}
	virtual bool has_parent_object() { return false; }
	virtual	void on_physics_disable() {}
	virtual xrPhysX::Interfaces::IPHCapture* PHCapture() { return nullptr;}
	virtual	bool IsInventoryItem() { return false; }
	virtual	bool IsActor() { return false; }
 	virtual bool IsStalker() { return false; }
	//virtual	void						SetWeaponHideState					( u16 State, bool bSet )=0;
	virtual	void HideAllWeapons(bool v) {}//(SetWeaponHideState(INV_STATE_BLOCK_ALL,true))
	virtual	void MovementCollisionEnable(bool enable) {}
	virtual xrPhysX::Interfaces::IPHSoundPlayer* ObjectPhSoundPlayer() { return nullptr; }
	virtual xrPhysX::Interfaces::ICollisionDamageReceiver* ObjectPhCollisionDamageReceiver() { return nullptr; }
	virtual	void BonceDamagerCallback(float &damage_factor){}
public:
    virtual const Fmatrix& ObjectXFORM() const override { return m_object_xform; }
private:
    virtual	const Fvector& ObjectPosition() const { return m_object_xform.c; }

#ifdef	DEBUG
	virtual	xr_string dump(xrPhysX::EDumpType type) const { VERIFY(false); return xr_string("ActorEditor!");}
#endif
};
 