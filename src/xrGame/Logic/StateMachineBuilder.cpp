#include "stdafx.h"
#include "StateMachineBuilder.h"
#include "TemplateStateBuilder.h"
#include "ActualStateBuilder.h"

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
    VERIFY4(Ltx->section_exist(StartState), "There is not required state in file", StartState, Ltx->fname());

    return nullptr;
}
