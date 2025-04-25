#include "stdafx.h"
#include "WeaponMagazine.h"

void Weapon::CreateMagazine(EWeaponMagazine Type, xr_unique_ptr<IWeaponMagazine>& Out)
{
    static xr_hash_map<EWeaponMagazine, xr_unique_ptr<IWeaponMagazine>> Templates = {

    };
    auto it = Templates.find(Type);
    R_ASSERT(it != Templates.end());
    it->second->CreateInstance(Out);
}

ISaveObject& operator<<(ISaveObject& Object, EWeaponMagazine& Value)
{
    u8 temp = (u8)Value;
    Object << temp;
    Value = (EWeaponMagazine)temp;
    return Object;
}
