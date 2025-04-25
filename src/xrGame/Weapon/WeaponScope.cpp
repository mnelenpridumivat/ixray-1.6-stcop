#include "stdafx.h"
#include "WeaponScope.h"

ISaveObject& operator<<(ISaveObject& Object, EWeaponScope& Value)
{
    u8 temp = (u8)Value;
    Object << temp;
    Value = (EWeaponScope)temp;
    return Object;
}

void Weapon::CreateScope(EWeaponScope Type, xr_unique_ptr<IWeaponScope>& Out)
{
    static xr_hash_map<EWeaponScope, xr_unique_ptr<IWeaponScope>> Templates = {

    };
    auto it = Templates.find(Type);
    R_ASSERT(it != Templates.end());
    it->second->CreateInstance(Out);
}
