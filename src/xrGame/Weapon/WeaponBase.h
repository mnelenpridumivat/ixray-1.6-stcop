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

    xr_unique_ptr<IWeaponMagazine> Magazine;
    xr_unique_ptr<IWeaponBarrel> Barrel;
    xr_unique_ptr<IWeaponUnderbarrel> Underbarrel;
    xr_unique_ptr<IWeaponScope> PrimaryScope;
    xr_unique_ptr<IWeaponScope> SecondaryScope;
    
public:
	virtual void Serialize(ISaveObject& Object) override;
	virtual void Load(LPCSTR section) override;
    
};
