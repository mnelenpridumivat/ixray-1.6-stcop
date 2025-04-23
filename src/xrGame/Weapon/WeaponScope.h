#pragma once

class IWeaponScope
{
public:
	virtual void Serialize(ISaveObject& Object) = 0;
    
};
