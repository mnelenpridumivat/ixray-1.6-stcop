#pragma once
#include "StateBuilder.h"

class Template;

class CTemplateStateBuilder :
    public CStateBuilder
{

	xr_map<shared_str, xr_unique_ptr<Template>> TemplateSections;


public:
	static bool IsTemplateSection(LPCSTR SectionName);
	virtual void PreprocessFile(CInifile* Ltx) override;
	virtual xr_unique_ptr<CState>&& CreateState(CInifile* Ltx, shared_str StateName, xr_deque<shared_str>& NextStates) override;

};

