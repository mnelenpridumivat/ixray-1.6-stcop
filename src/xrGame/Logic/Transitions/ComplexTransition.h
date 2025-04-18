#pragma once
#include "Transition.h"

namespace Logic
{
	class CState;
	class CCondition;

	class CComplexTransition : public CTransition
	{
	private:
		xr_vector<CCondition*> conditions;
		xr_variant<CState*, shared_str> NextState;

	public:
		
        static CTransition* Create();  

		virtual void OnTransitionActivate() override;
		virtual void OnTransitionDeactivate() override;

		virtual bool CheckConditions() override;

		void SetNextState(shared_str nextState);
		void SetNextState(CState* nextState);

	};
}

