#pragma once
#include "StateBuilder.h"
class CActualStateBuilder :
    public CStateBuilder
{
public:

	virtual void PreprocessFile(CInifile* Ltx) override;
	virtual CState* CreateState(CInifile* Ltx, LPCSTR StateName, xr_vector<shared_str>& NextStates) override;
};

