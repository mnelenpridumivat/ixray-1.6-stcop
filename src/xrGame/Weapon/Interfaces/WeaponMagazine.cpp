#include "stdafx.h"
#include "WeaponMagazine.h"

IWeaponMagazine* Weapon::CreateMagazine(EWeaponMagazine Type)
{
    static xr_hash_map<EWeaponMagazine, IWeaponMagazine*> Templates = {

    };
    auto it = Templates.find(Type);
    R_ASSERT(it != Templates.end());
    return it->second->CreateInstance();
}

ISaveObject& operator<<(ISaveObject& Object, EWeaponMagazine& Value)
{
    u8 temp = (u8)Value;
    Object << temp;
    Value = (EWeaponMagazine)temp;
    return Object;
}
