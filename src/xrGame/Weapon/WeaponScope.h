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
	virtual void CreateInstance(xr_unique_ptr<IWeaponScope>& out);
    
};

namespace Weapon
{
	void CreateScope(EWeaponScope Type, xr_unique_ptr<IWeaponScope>& Out);
}
