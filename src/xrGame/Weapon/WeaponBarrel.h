#pragma once

enum class EWeaponBarrel: u8
{
	Invalid
};

ISaveObject& operator<<(ISaveObject& Object, EWeaponBarrel& Value);

class IWeaponBarrel
{
public:
	virtual void Serialize(ISaveObject& Object) = 0;
	virtual void Load(LPCSTR section) = 0;

    virtual bool CanContainAmmo() = 0;

	virtual EWeaponBarrel GetType() = 0;
	virtual void CreateInstance(xr_unique_ptr<IWeaponBarrel>& Out) = 0;
    
};

namespace Weapon
{
	void CreateBarrel(EWeaponBarrel Type, xr_unique_ptr<IWeaponBarrel>& Out);
}
