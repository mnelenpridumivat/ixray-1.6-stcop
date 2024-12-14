#include "stdafx.h"
#include "net_physics_state.h"
#include "Save/SaveObject.h"

void net_physics_state::fill(SPHNetState &state, u32 time)
{
	dwTimeStamp = time;
	physics_linear_velocity = state.linear_vel;
	physics_position = state.position;
	physics_state_enabled = state.enabled;
}

void net_physics_state::write(NET_Packet &packet)
{
	packet << dwTimeStamp;
	packet << physics_position;
	packet << physics_state_enabled;
}

void net_physics_state::read(NET_Packet &packet)
{
	packet >> dwTimeStamp;
	packet >> physics_position;
	packet >> physics_state_enabled;

	physics_linear_velocity.set(0, 0, 0);
}

void net_physics_state::serialize(ISaveObject& Object)
{
	Object.BeginChunk("net_physics_state");
	{
		Object << dwTimeStamp << physics_position << physics_state_enabled;
	}
	Object.EndChunk();
}

/*void net_physics_state::write(CSaveObjectSave* Object)
{
	Object->BeginChunk("net_physics_state");
	{
		Object->GetCurrentChunk()->w_u32(dwTimeStamp);
		Object->GetCurrentChunk()->w_vec3(physics_position);
		Object->GetCurrentChunk()->w_bool(physics_state_enabled);
	}
	Object->EndChunk();
}

void net_physics_state::read(CSaveObjectLoad* Object)
{
	Object->BeginChunk("net_physics_state");
	{
		Object->GetCurrentChunk()->r_u32(dwTimeStamp);
		Object->GetCurrentChunk()->r_vec3(physics_position);
		Object->GetCurrentChunk()->r_bool(physics_state_enabled);
	}
	Object->EndChunk();
}*/
