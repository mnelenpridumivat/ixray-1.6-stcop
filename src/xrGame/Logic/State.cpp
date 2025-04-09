#include "stdafx.h"
#include "State.h"
#include "Transition.h"

void CState::OnStateActivate()
{
	for (auto& elem : Transitions) {
		elem->OnTransitionActivate();
	}
}

void CState::OnStateDeactivate()
{
	for (auto& elem : Transitions) {
		elem->OnTransitionDeactivate();
	}
}
