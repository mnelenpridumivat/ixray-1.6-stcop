#include "stdafx.h"
#include "ComplexTransition.h"
#include "Condition.h"

using namespace Logic;

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

CTransition* CTransition::Create()
{
	return new CTransition();
}

void CTransition::SetNextState(shared_str nextState)
{
	VERIFY(NextState.index() != 0);
	NextState = nextState;
}

void CTransition::SetNextState(CState* nextState)
{
	NextState = nextState;
}
