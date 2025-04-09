#include "stdafx.h"
#include "StateMachineBuilder.h"
#include "TemplateStateBuilder.h"
#include "ActualStateBuilder.h"

CStateBuilder* CStateMachineBuilder::GetStateBuilderBySectionName(shared_str SectionName)
{
    return CTemplateStateBuilder::IsTemplateSection(SectionName.c_str()) ? (CStateBuilder*)(TemplateStateBuilder.get()) : ActualStateBuilder.get();
}

CStateMachineBuilder::CStateMachineBuilder()
{
    TemplateStateBuilder = xr_make_unique<CTemplateStateBuilder>();
    ActualStateBuilder = xr_make_unique<CActualStateBuilder>();
}

void CStateMachineBuilder::PreprocessFile(CInifile* Ltx)
{
    TemplateStateBuilder->PreprocessFile(Ltx);
    ActualStateBuilder->PreprocessFile(Ltx);
}

CStateMachine* CStateMachineBuilder::CreateStateMachine(CInifile* Ltx, LPCSTR StartState)
{
    R_ASSERT(Ltx);

    StatesToCreate.push_back(StartState);
    while (!StatesToCreate.empty())
    {
        auto State = StatesToCreate.front();
        StatesToCreate.pop_front();
        auto Builder = GetStateBuilderBySectionName(State);
        ConstructedStates[State] = Builder->CreateState(Ltx, State, StatesToCreate);
    }
}
