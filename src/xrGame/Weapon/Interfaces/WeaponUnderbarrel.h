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
	virtual IWeaponUnderbarrel* CreateInstance() = 0;
    
};

namespace Weapon
{
	IWeaponUnderbarrel* CreateUnderbarrel(EWeaponUnderbarrel Type);
}
