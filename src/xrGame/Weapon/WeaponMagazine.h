#pragma once

class IWeaponMagazine
{
public:
	virtual void Serialize(ISaveObject& Object) = 0;

    virtual bool IsEmpty() = 0;
    
};
