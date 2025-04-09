#include "stdafx.h"
#include "Transition.h"
#include "Condition.h"

void CTransition::OnTransitionActivate()
{
	for (auto& elem : conditions) {
		elem->OnConditionActivate();
	}
}

void CTransition::OnTransitionDeactivate()
{
	for (auto& elem : conditions) {
		elem->OnConditionDeactivate();
	}
}

bool CTransition::CheckConditions()
{
	bool Result = true;
	for (const auto& elem : conditions) {
		Result = Result && elem->GetIsSucced();
	}
	return Result;
}
