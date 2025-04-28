#pragma once
#include "hud_item_object.h"
#include "ShootingObject.h"

class IWeaponUnderbarrel;
class IWeaponScope;
class IWeaponBarrel;
class IWeaponMagazine;

class CWeaponBase : public CHudItemObject,
                    public CShootingObject
{
    typedef CHudItemObject inherited;

    IWeaponMagazine* Magazine = nullptr;
    IWeaponBarrel* Barrel = nullptr;
    IWeaponUnderbarrel* Underbarrel = nullptr;
    IWeaponScope* PrimaryScope = nullptr;
    IWeaponScope* SecondaryScope = nullptr;
    
public:
	virtual ~CWeaponBase();
	virtual void Serialize(ISaveObject& Object) override;
	virtual void Load(LPCSTR section) override;
    
};
