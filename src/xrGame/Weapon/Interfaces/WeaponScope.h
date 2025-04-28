#pragma once

enum class EWeaponScope
{
	Invalid
};

ISaveObject& operator<<(ISaveObject& Object, EWeaponScope& Value);

class IWeaponScope
{
public:
	virtual void Serialize(ISaveObject& Object) = 0;
	virtual void Load(LPCSTR section) = 0;

	virtual EWeaponScope GetType() = 0;
	virtual IWeaponScope* CreateInstance();
    
};

namespace Weapon
{
	IWeaponScope* CreateScope(EWeaponScope Type);
}
