#include "stdafx.h"
#include "WeaponUnderbarrel.h"

IWeaponUnderbarrel* Weapon::CreateUnderbarrel(EWeaponUnderbarrel Type)
{
    static xr_hash_map<EWeaponUnderbarrel, IWeaponUnderbarrel*> Templates = {

    };
    auto it = Templates.find(Type);
    R_ASSERT(it != Templates.end());
    return it->second->CreateInstance();
}

ISaveObject& operator<<(ISaveObject& Object, EWeaponUnderbarrel& Value)
{
    u8 temp = (u8)Value;
    Object << temp;
    Value = (EWeaponUnderbarrel)temp;
    return Object;
}
