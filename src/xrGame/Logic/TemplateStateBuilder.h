#pragma once
#include "StateBuilder.h"

class Template;

class CTemplateStateBuilder :
    public CStateBuilder
{

	xr_map<shared_str, xr_unique_ptr<Template>> TemplateSections;

	bool IsTemplateSection(LPCSTR SectionName);

public:
	virtual void PreprocessFile(CInifile* Ltx) override;
	virtual CState* CreateState(CInifile* Ltx, LPCSTR StateName, xr_vector<shared_str>& NextStates) override;

};

