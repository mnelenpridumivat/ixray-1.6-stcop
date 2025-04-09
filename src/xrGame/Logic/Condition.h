#pragma once

class CCondition
{
private:
	bool IsSucced = false;

public:
	CCondition(bool InitialCondition) : IsSucced(InitialCondition) {};

	bool GetIsSucced() const { return IsSucced; }
	void SetIsSucced(bool IsSucced) { this->IsSucced = IsSucced; }

	virtual void OnConditionActivate();
	virtual void OnConditionDeactivate();
};

