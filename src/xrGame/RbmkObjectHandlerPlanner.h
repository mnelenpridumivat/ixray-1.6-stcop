#pragma once
#include "GOAP/RbmkGoapPlanner.h"

class FRbmkObjectActionObjectActions;
class CAI_Stalker;
class CGameObject;
class CMissile;
class CWeapon;
class CObject;

class FRbmkObjectHandlerPlanner
{
public:
														FRbmkObjectHandlerPlanner	(void* InOwner);
	void												Initialize					();
	void												AddObject					(CInventoryItem*InventoryItem);
	void												AddObject					(CWeapon*Weapon);
	void												AddObject					(CMissile*Missile);
	void												RemoveObject				(CObject*Item);
	void												SetGoap						(MonsterSpace::EObjectAction ObjectAction, CGameObject *InGameObject, u32 InMinQueueSize, u32 InMaxQueueSize, u32 InMinQueueInterval, u32 InMaxQueueInterval);
	CAI_Stalker*										GetOwner					() const	{ return static_cast<CAI_Stalker*>(GoapPlanner.Owner); }
	uint16												CurrentActionObjectID		() const;
	shared_str											CurrentActionStateName		() const;
	bool												IsWeaponGoingToBeStrapped	(const CGameObject* GameObject) const;
	uint32												GetAimTime					(const CWeapon* GameObject) const;
	void												SetAimTime					(const CWeapon* GameObject,uint32 NewAimTime) ;
								
	FRbmkGoapPlanner									GoapPlanner;
							
	bool 												bAimed1;
	bool 												bAimed2;
	bool 												bStrapped;
	bool 												bStrapped2Idle;
	bool 												bUseEnough;
	
	xr_hash_map<CObject*,FRbmkGoapProperty*>			ItemProperties;
	xr_hash_map<CObject*,FRbmkObjectActionObjectActions*>ItemActions;

	uint32												MinQueueSize;
	uint32												MaxQueueSize;
	uint32												MinQueueInterval;
	uint32												MaxQueueInterval;
	uint32												QueueInterval;
	uint32												NextTimeChange;
	CGameObject*										TargetObject = nullptr;
	
};
