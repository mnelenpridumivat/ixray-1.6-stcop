#pragma once

namespace Logic
{
	class CStateMachine;
	class CStateBuilder;
	class CTemplateStateBuilder;
	class CActualStateBuilder;
	class CState;

	class CStateMachineBuilder
	{

		xr_unique_ptr<CTemplateStateBuilder> TemplateStateBuilder;
		xr_unique_ptr<CActualStateBuilder> ActualStateBuilder;

		xr_hash_map<shared_str, xr_unique_ptr<CState>> ConstructedStates;
		xr_deque<shared_str> StatesToCreate;

		CStateBuilder* GetStateBuilderBySectionName(shared_str SectionName);

	public:
		CStateMachineBuilder();

		void PreprocessFile(CInifile* Ltx);
		CStateMachine* CreateStateMachine(CInifile* Ltx, LPCSTR StartState);

	};
}
