#pragma once

class IWeaponBarrel
{
public:
	virtual void Serialize(ISaveObject& Object) = 0;

    virtual bool CanContainAmmo() = 0;
    
};
