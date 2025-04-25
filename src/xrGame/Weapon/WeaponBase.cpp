#include "stdafx.h"
#include "WeaponBase.h"

#include <magic_enum/magic_enum.hpp>

#include "WeaponBarrel.h"
#include "WeaponMagazine.h"
#include "WeaponScope.h"
#include "WeaponUnderbarrel.h"

void CWeaponBase::Serialize(ISaveObject& Object)
{
    BEGIN_CHUNK(Object, "CWeaponBase::Serialize")
    {
        CHudItemObject::Serialize(Object);
        BEGIN_CHUNK(Object, "CWeaponBase::Magazine")
        {
            EWeaponMagazine type = Object.IsSave() ? Magazine->GetType() : EWeaponMagazine::Invalid;
            BEGIN_CHUNK(Object, "CWeaponBase::Magazine::ID")
            {
                Object << type;
            }
            VERIFY(type != EWeaponMagazine::Invalid);
            /*if(!Object.IsSave())
            {
                Weapon::CreateMagazine(type, Magazine);
            }*/
            Magazine->Serialize(Object);
        }
        BEGIN_CHUNK(Object, "CWeaponBase::Barrel")
        {
            EWeaponBarrel type = Object.IsSave() ? Barrel->GetType() : EWeaponBarrel::Invalid;
            BEGIN_CHUNK(Object, "CWeaponBase::Barrel::ID")
            {
                Object << type;
            }
            VERIFY(type != EWeaponBarrel::Invalid);
            /*if(!Object.IsSave())
            {
                Weapon::CreateBarrel(type, Barrel);
            }*/
            Barrel->Serialize(Object);
        }
        BEGIN_CHUNK(Object, "CWeaponBase::Underbarrel")
        {
            EWeaponUnderbarrel type = Object.IsSave() ? Underbarrel->GetType() : EWeaponUnderbarrel::Invalid;
            BEGIN_CHUNK(Object, "CWeaponBase::Underbarrel::ID")
            {
                Object << type;
            }
            VERIFY(type != EWeaponUnderbarrel::Invalid);
            /*if(!Object.IsSave())
            {
                Weapon::CreateUnderbarrel(type, Underbarrel);
            }*/
            Underbarrel->Serialize(Object);
        }
        BEGIN_CHUNK(Object, "CWeaponBase::PrimaryScope")
        {
            EWeaponScope type = Object.IsSave() ? PrimaryScope->GetType() : EWeaponScope::Invalid;
            BEGIN_CHUNK(Object, "CWeaponBase::PrimaryScope::ID")
            {
                Object << type;
            }
            VERIFY(type != EWeaponScope::Invalid);
            /*if(!Object.IsSave())
            {
                Weapon::CreateScope(type, PrimaryScope);
            }*/
            PrimaryScope->Serialize(Object);
        }
        BEGIN_CHUNK(Object, "CWeaponBase::SecondaryScope")
        {
            EWeaponScope type = Object.IsSave() ? SecondaryScope->GetType() : EWeaponScope::Invalid;
            BEGIN_CHUNK(Object, "CWeaponBase::SecondaryScope::ID")
            {
                Object << type;
            }
            VERIFY(type != EWeaponScope::Invalid);
            /*if(!Object.IsSave())
            {
                Weapon::CreateScope(type, SecondaryScope);
            }*/
            SecondaryScope->Serialize(Object);
        }
    }
}

void CWeaponBase::Load(LPCSTR section)
{
    CHudItemObject::Load(section);
    {
        auto ConfigType = READ_IF_EXISTS(pSettings, r_string, section, "magazine_type", "Invalid");
        auto RetType = magic_enum::enum_cast<EWeaponMagazine>(ConfigType);
        R_ASSERT3(RetType.has_value(), "Invalid magazine_type", ConfigType);
        EWeaponMagazine type = RetType.value_or(EWeaponMagazine::Invalid);
        R_ASSERT2(type != EWeaponMagazine::Invalid, "magazine_type cannot be Invalid");
        Weapon::CreateMagazine(type, Magazine);
    }
    {
        auto ConfigType = READ_IF_EXISTS(pSettings, r_string, section, "barrel_type", "Invalid");
        auto RetType = magic_enum::enum_cast<EWeaponBarrel>(ConfigType);
        R_ASSERT3(RetType.has_value(), "Invalid barrel_type", ConfigType);
        EWeaponBarrel type = RetType.value_or(EWeaponBarrel::Invalid);
        R_ASSERT2(type != EWeaponBarrel::Invalid, "barrel_type cannot be Invalid");
        Weapon::CreateBarrel(type, Barrel);
    }
    {
        auto ConfigType = READ_IF_EXISTS(pSettings, r_string, section, "underbarrel_type", "Invalid");
        auto RetType = magic_enum::enum_cast<EWeaponUnderbarrel>(ConfigType);
        R_ASSERT3(RetType.has_value(), "Invalid underbarrel_type", ConfigType);
        EWeaponUnderbarrel type = RetType.value_or(EWeaponBarrel::Invalid);
        R_ASSERT2(type != EWeaponUnderbarrel::Invalid, "underbarrel_type cannot be Invalid");
        Weapon::CreateUnderbarrel(type, Underbarrel);
    }
    {
        auto ConfigType = READ_IF_EXISTS(pSettings, r_string, section, "primary_scope_type", "Invalid");
        auto RetType = magic_enum::enum_cast<EWeaponScope>(ConfigType);
        R_ASSERT3(RetType.has_value(), "Invalid primary_scope_type", ConfigType);
        EWeaponScope type = RetType.value_or(EWeaponScope::Invalid);
        R_ASSERT2(type != EWeaponScope::Invalid, "primary_scope_type cannot be Invalid");
        Weapon::CreateScope(type, PrimaryScope);
    }
    {
        auto ConfigType = READ_IF_EXISTS(pSettings, r_string, section, "secondary_scope_type", "Invalid");
        auto RetType = magic_enum::enum_cast<EWeaponScope>(ConfigType);
        R_ASSERT3(RetType.has_value(), "Invalid secondary_scope_type", ConfigType);
        EWeaponScope type = RetType.value_or(EWeaponScope::Invalid);
        R_ASSERT2(type != EWeaponScope::Invalid, "secondary_scope_type cannot be Invalid");
        Weapon::CreateScope(type, SecondaryScope);
    }
}
