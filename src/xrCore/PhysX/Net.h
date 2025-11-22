#pragma once
#include "../xrServerEntities/PHNetState.h"

namespace xrPhysX::Net {
    class XRCORE_API net_physics_state
    {
    public:
    	Fvector physics_linear_velocity = { 0, 0, 0 };
    	Fvector physics_position = { 0, 0, 0 };
    	bool	physics_state_enabled = false;
    	u32		dwTimeStamp = 0;
    
    public:
    	void fill(SPHNetState &state, u32 time);
    	void write(NET_Packet &packet);
    	void read(NET_Packet &packet);
    	void serialize(ISaveObject& Object);
    };
}