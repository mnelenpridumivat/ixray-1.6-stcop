#pragma once

class CCondition;

class CTransition
{
private:
	xr_vector<CCondition*> conditions;

public:

	virtual void OnTransitionActivate();
	virtual void OnTransitionDeactivate();

	bool CheckConditions();

};

