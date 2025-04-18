#pragma once

namespace Logic
{
	class CState;

	class CStateMachine
	{
	private:

		xr_hash_map<shared_str, xr_unique_ptr<CState>> States;

	};
}

