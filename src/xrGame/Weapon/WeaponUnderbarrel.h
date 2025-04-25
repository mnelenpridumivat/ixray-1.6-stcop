#pragma once

enum class EWeaponUnderbarrel : u8
{
	Invalid,
};

ISaveObject& operator<<(ISaveObject& Object, EWeaponUnderbarrel& Value);

class IWeaponUnderbarrel
{
public:
	virtual void Serialize(ISaveObject& Object) = 0;
	virtual void Load(LPCSTR section) = 0;

	virtual EWeaponUnderbarrel GetType() = 0;
	virtual void CreateInstance(xr_unique_ptr<IWeaponUnderbarrel>& Out) = 0;
    
};

namespace Weapon
{
	void CreateUnderbarrel(EWeaponUnderbarrel Type, xr_unique_ptr<IWeaponUnderbarrel>& Out);
}
