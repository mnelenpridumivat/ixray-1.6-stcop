#pragma once

#include "inventory_item_object.h"
#include "../xrScripts/script_export_space.h"

class CMagazine : 
    public CInventoryItemObject 
{
private:
    typedef CInventoryItemObject inherited;
public:
    CMagazine ();
    virtual ~CMagazine();
};