#pragma once

enum class EWeaponBarrel: u8
{
	Invalid,
	Rifle
};

ISaveObject& operator<<(ISaveObject& Object, EWeaponBarrel& Value);

class IWeaponBarrel
{
public:
	virtual ~IWeaponBarrel() = default;
	virtual void Serialize(ISaveObject& Object) = 0;
	virtual void Load(LPCSTR section) = 0;

    virtual bool CanContainAmmo() = 0;

	virtual EWeaponBarrel GetType() = 0;
	virtual IWeaponBarrel* CreateInstance() = 0;
    
};

namespace Weapon
{
	IWeaponBarrel* CreateBarrel(EWeaponBarrel Type);
}
