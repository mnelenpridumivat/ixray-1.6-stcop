#include "stdafx.h"
#include "WeaponBarrel.h"

void Weapon::CreateBarrel(EWeaponBarrel Type, xr_unique_ptr<IWeaponBarrel>& Out)
{
    static xr_hash_map<EWeaponBarrel, xr_unique_ptr<IWeaponBarrel>> Templates = {

    };
    auto it = Templates.find(Type);
    R_ASSERT(it != Templates.end());
    it->second->CreateInstance(Out);
}

ISaveObject& operator<<(ISaveObject& Object, EWeaponBarrel& Value)
{
    u8 temp = (u8)Value;
    Object << temp;
    Value = (EWeaponBarrel)temp;
    return Object;
}
