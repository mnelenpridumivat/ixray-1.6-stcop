#include "stdafx.h"
#include "WeaponBarrelRifle.h"

void CWeaponBarrelRifle::Serialize(ISaveObject& Object)
{
    BEGIN_CHUNK(Object, "CWeaponBarrelRifle")
    {
        
    }
}

void CWeaponBarrelRifle::Load(LPCSTR section)
{
}

bool CWeaponBarrelRifle::CanContainAmmo()
{
    return true;
}

EWeaponBarrel CWeaponBarrelRifle::GetType()
{
    return EWeaponBarrel::Rifle;
}

IWeaponBarrel* CWeaponBarrelRifle::CreateInstance()
{
    return new CWeaponBarrelRifle();
}
