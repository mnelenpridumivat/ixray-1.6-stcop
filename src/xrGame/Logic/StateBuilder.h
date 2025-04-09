#pragma once

class CState;

class CStateBuilder
{
protected:
	
	
public:

	virtual void PreprocessFile(CInifile* Ltx) = 0;
	virtual xr_unique_ptr<CState>&& CreateState(CInifile* Ltx, shared_str StateName, xr_deque<shared_str>& NextStates) = 0;

};

