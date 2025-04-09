#pragma once

class CState;

class CStateBuilder
{
public:

	virtual void PreprocessFile(CInifile* Ltx) = 0;
	virtual CState* CreateState(CInifile* Ltx, LPCSTR StateName, xr_vector<shared_str>& NextStates) = 0;

};

