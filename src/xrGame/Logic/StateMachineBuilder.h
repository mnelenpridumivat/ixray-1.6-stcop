#pragma once

class CStateMachine;
class CStateBuilder;
class CTemplateStateBuilder;
class CActualStateBuilder;

class CStateMachineBuilder
{

	xr_unique_ptr<CTemplateStateBuilder> TemplateStateBuilder;
	xr_unique_ptr<CActualStateBuilder> ActualStateBuilder;

	CStateBuilder* GetStateBuilderBySectionName(LPCSTR SectionName);

public:
	CStateMachineBuilder();

	void PreprocessFile(CInifile* Ltx);
	CStateMachine* CreateStateMachine(CInifile* Ltx, LPCSTR StartState);

};

