#pragma once

class CTransition;

class CState
{
private:

	xr_vector<xr_unique_ptr<CTransition>> Transitions;

public:

	virtual void OnStateActivate();
	virtual void OnStateDeactivate();

};

