#pragma once

enum class EWeaponMagazine: u8
{
	Invalid,
};

ISaveObject& operator<<(ISaveObject& Object, EWeaponMagazine& Value);

class IWeaponMagazine
{
public:
	virtual void Serialize(ISaveObject& Object) = 0;
	virtual void Load(LPCSTR section) = 0;

    virtual bool IsEmpty() = 0;
	
	virtual EWeaponMagazine GetType() = 0;
    virtual IWeaponMagazine* CreateInstance() = 0;
};

namespace Weapon
{
	IWeaponMagazine* CreateMagazine(EWeaponMagazine Type);
}
