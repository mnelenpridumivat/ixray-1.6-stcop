#include "stdafx.h"
#include "WeaponUnderbarrel.h"

void Weapon::CreateUnderbarrel(EWeaponUnderbarrel Type, xr_unique_ptr<IWeaponUnderbarrel>& Out)
{
    static xr_hash_map<EWeaponUnderbarrel, xr_unique_ptr<IWeaponUnderbarrel>> Templates = {

    };
    auto it = Templates.find(Type);
    R_ASSERT(it != Templates.end());
    it->second->CreateInstance(Out);
}

ISaveObject& operator<<(ISaveObject& Object, EWeaponUnderbarrel& Value)
{
    u8 temp = (u8)Value;
    Object << temp;
    Value = (EWeaponUnderbarrel)temp;
    return Object;
}
