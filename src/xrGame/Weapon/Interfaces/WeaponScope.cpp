#include "stdafx.h"
#include "WeaponScope.h"

ISaveObject& operator<<(ISaveObject& Object, EWeaponScope& Value)
{
    u8 temp = (u8)Value;
    Object << temp;
    Value = (EWeaponScope)temp;
    return Object;
}

IWeaponScope* Weapon::CreateScope(EWeaponScope Type)
{
    static xr_hash_map<EWeaponScope, IWeaponScope*> Templates = {

    };
    auto it = Templates.find(Type);
    R_ASSERT(it != Templates.end());
    return it->second->CreateInstance();
}
