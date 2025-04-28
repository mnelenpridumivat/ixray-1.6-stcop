#pragma once
#include "Interfaces/WeaponBarrel.h"

class CWeaponBarrelRifle : public IWeaponBarrel
{
public:
    void Serialize(ISaveObject& Object) override;
    void Load(LPCSTR section) override;
    bool CanContainAmmo() override;
    EWeaponBarrel GetType() override;
    IWeaponBarrel* CreateInstance() override;
};