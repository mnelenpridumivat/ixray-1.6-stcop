#pragma once

namespace Logic
{
	class CTransition;
	class CState;

	class CStateBuilder
	{
	protected:

		CTransition* CreateTransition(const xr_string MainCond, xr_string SecondaryConds, const xr_string& NextState, xr_string Actions);
	
	public:

		virtual void PreprocessFile(CInifile* Ltx) = 0;
		virtual xr_unique_ptr<CState>&& CreateState(CInifile* Ltx, shared_str StateName, xr_deque<shared_str>& NextStates) = 0;

	};
}

