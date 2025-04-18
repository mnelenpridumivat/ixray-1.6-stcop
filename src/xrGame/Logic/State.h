#pragma once

namespace Logic
{
	class CTransition;
	class CActualStateBuilder;
	class CTemplateStateBuilder;

	class CState
	{
		friend class CActualStateBuilder;
		friend class CTemplateStateBuilder;

		shared_str StateName;
		xr_hash_map<shared_str, xr_variant<shared_str, double>> Mappings;
		xr_vector<xr_unique_ptr<CTransition>> Transitions;

	public:

		virtual void OnStateActivate();
		virtual void OnStateDeactivate();

	};
}

