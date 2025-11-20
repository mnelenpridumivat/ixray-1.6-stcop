#pragma once

#include "GameObject.h"
#include "ParticlesPlayer.h"
#include "../xrEngine/IObjectPhysicsCollision.h"
#ifndef IXRAY_PHYSX
#include "../xrPhysics/IPhysicsShellHolder.h"
#endif
#include "PHCollisionDamageReceiver.h"
#include "../xrScripts/script_export_space.h"
#include "../xrCore/Save/SaveObject.h"
#include "PhysX/Interfaces/CollisionDamageReceiver.h"
#include "PhysX/Interfaces/CollisionHitCallback.h"
#include "PhysX/Interfaces/PHCapture.h"
#include "PhysX/Wrappers/Shell.h"

class CPHDestroyable;
class CPHCollisionDamageReceiver;
class CPHSoundPlayer;
class IDamageSource;
class CPHSkeleton;
class CCharacterPhysicsSupport;
class ICollisionDamageInfo;
class CIKLimbsController;
class CSaveObjectSave;
class CSaveObjectLoad;



class CPhysicsShellHolder:  public CGameObject,
							public CParticlesPlayer,
							public IObjectPhysicsCollision,
							public xrPhysX::Interfaces::IShellHolder
	
{
	bool				b_sheduled;
public:
	void	SheduleRegister		(){if(!IsSheduled())shedule_register();b_sheduled=true;}
	void	SheduleUnregister	(){if(IsSheduled())shedule_unregister();b_sheduled=false;}
IC	bool	IsSheduled			(){return b_sheduled;}	
public:

	typedef CGameObject inherited;


	xrPhysX::Wrappers::CShell* m_pPhysicsShell = nullptr;


			CPhysicsShellHolder							();
	virtual	~CPhysicsShellHolder						();

private:
	Fvector					m_overriden_activation_speed;
	bool					m_activation_speed_is_overriden;

public:
	virtual bool ActivationSpeedOverriden(Fvector& dest, bool clear_override);
	virtual void SetActivationSpeedOverride(Fvector const& speed);

	xrPhysX::Wrappers::CShell* PPhysicsShell				()		const
	{
		return m_pPhysicsShell;
	}

	void SetPPhysicsShell(xrPhysX::Wrappers::CShell* pp) { m_pPhysicsShell = pp; }

	IC CPhysicsShellHolder*	PhysicsShellHolder	()
	{
		return this;
	}
	virtual	const IObjectPhysicsCollision* physics_collision();
	virtual	const IPhysicsShell* physics_shell() const;
	virtual IPhysicsShell* physics_shell();
	virtual const IPhysicsElement* physics_character() const;
	virtual CPHDestroyable* ph_destroyable() { return nullptr; }
	virtual xrPhysX::Interfaces::ICollisionDamageReceiver* PHCollisionDamageReceiver() { return nullptr; }
	virtual CPHSkeleton* PHSkeleton() { return nullptr; }

	virtual CPhysicsShellHolder* cast_physics_shell_holder() { return this; }
	virtual CPhysicItem* cast_physics_item() { return nullptr; }
	virtual CParticlesPlayer* cast_particles_player() { return this; }
	virtual CGameObject* cast_game_object() { return this; }
	virtual IDamageSource* cast_IDamageSource() { return nullptr; }

	virtual CPHSoundPlayer* ph_sound_player() { return nullptr; }
	virtual	CCharacterPhysicsSupport* character_physics_support() { return nullptr; }
	virtual	const CCharacterPhysicsSupport* character_physics_support() const { return nullptr; }
	virtual	CIKLimbsController* character_ik_controller() { return nullptr; }
	virtual xrPhysX::Interfaces::ICollisionHitCallback* get_collision_hit_callback() { return nullptr; }

	virtual void set_collision_hit_callback(xrPhysX::Interfaces::ICollisionHitCallback* cc) {}
	virtual void _BCL enable_notificate() {}
public:

	virtual void PHGetLinearVell(Fvector& velocity);
	virtual void PHSetLinearVell(Fvector& velocity);
	virtual void PHSetMaterial(LPCSTR m);
	virtual void PHSetMaterial(u16 m);
	void PHSaveState(NET_Packet &P);
	void PHLoadState(IReader &P);
			//void			PHSaveState(CSaveObjectSave* Object) const;
			//void			PHLoadState(CSaveObjectLoad* Object);
	void PHSerializeState(ISaveObject& Object);
	virtual f32	GetMass();
	virtual	void PHHit(SHit &H);
	virtual	void Hit(SHit* pHDS);
///////////////////////////////////////////////////////////////////////
	virtual u16	PHGetSyncItemsNumber() const;
	virtual CPHSynchronize*	PHGetSyncItem(u16 item) const;
	virtual void PHUnFreeze();
	virtual void PHFreeze();
	virtual float EffectiveGravity();
///////////////////////////////////////////////////////////////
	virtual void create_physic_shell();
	virtual void activate_physic_shell();
	virtual void setup_physic_shell();
	virtual void deactivate_physics_shell();

	virtual void Load(LPCSTR Section) override { inherited::Load(Section); }

	virtual void net_Destroy();
	virtual BOOL net_Spawn(CSE_Abstract*	DC);
	virtual void save(NET_Packet &output_packet);
	virtual void load(IReader &input_packet);
	//virtual void Save(CSaveObjectSave* Object) const override;
	//virtual void Load(CSaveObjectLoad* Object) override;
	virtual void Serialize(ISaveObject& Object) override;
	void init();

	virtual void OnChangeVisual();
	//для наследования CParticlesPlayer
	virtual void UpdateCL();
	void correct_spawn_pos();
	
protected:
	virtual	bool has_shell_collision_place( const CPhysicsShellHolder* obj ) const;
	virtual void on_child_shell_activate( CPhysicsShellHolder* obj );
public:
	virtual bool register_schedule() const;

public:
	virtual	void on_physics_disable();
private://IPhysicsShellHolder
	virtual	Fmatrix& ObjectXFORM();
	virtual	Fvector& ObjectPosition();
	virtual	LPCSTR ObjectName() const;
	virtual	LPCSTR ObjectNameVisual() const;
	virtual	LPCSTR ObjectNameSect() const;
	virtual	bool ObjectGetDestroy() const;
	virtual xrPhysX::Interfaces::ICollisionHitCallback* ObjectGetCollisionHitCallback();
	virtual	u16 ObjectID() const;
	virtual	ICollisionForm* ObjectCollisionModel();
	//virtual	IRenderVisual*			_BCL					ObjectVisual						()						;
	virtual	IKinematics* ObjectKinematics();
	virtual IDamageSource* ObjectCastIDamageSource();
	virtual	void ObjectProcessingDeactivate();
	virtual	void ObjectProcessingActivate();				
	virtual	void ObjectSpatialMove();
	virtual	xrPhysX::Wrappers::CShell* ObjectPPhysicsShell();
//	virtual	void						enable_notificate					()						;
	virtual bool has_parent_object();
//	virtual	void						on_physics_disable					()						;
	virtual xrPhysX::Interfaces::IPHCapture* PHCapture();
	virtual	bool IsInventoryItem();
	virtual	bool IsActor();
	virtual bool IsStalker();
	//virtual	void						SetWeaponHideState					( u16 State, bool bSet )=0;
	virtual	void HideAllWeapons( bool v );//(SetWeaponHideState(INV_STATE_BLOCK_ALL,true))
	virtual	void MovementCollisionEnable( bool enable )	;
	virtual CPHSoundPlayer* ObjectPhSoundPlayer() {return ph_sound_player();}
	virtual	ICollisionDamageReceiver* ObjectPhCollisionDamageReceiver();
	virtual	void BonceDamagerCallback(float &damage_factor);
#ifdef	DEBUG
	virtual	xr_string dump(xrPhysX::EDumpType type) const;
#endif
	DECLARE_SCRIPT_REGISTER_FUNCTION
};