#include "stdafx.h"
#include "WeaponBarrel.h"

#include "Weapon/WeaponBarrelRifle.h"

IWeaponBarrel* Weapon::CreateBarrel(EWeaponBarrel Type)
{
    static xr_hash_map<EWeaponBarrel, IWeaponBarrel* > Templates = {
        {EWeaponBarrel::Rifle, new CWeaponBarrelRifle()}
    };
    auto it = Templates.find(Type);
    R_ASSERT(it != Templates.end());
    return it->second->CreateInstance();
}

ISaveObject& operator<<(ISaveObject& Object, EWeaponBarrel& Value)
{
    u8 temp = (u8)Value;
    Object << temp;
    Value = (EWeaponBarrel)temp;
    return Object;
}
