#pragma once
#include "StateBuilder.h"
namespace Logic
{
	class CState;
	
	class CActualStateBuilder :
		public CStateBuilder
	{
	public:

		virtual void PreprocessFile(CInifile* Ltx) override;
		virtual xr_unique_ptr<CState>&& CreateState(CInifile* Ltx, shared_str StateName, xr_deque<shared_str>& NextStates) override;
	};
}

