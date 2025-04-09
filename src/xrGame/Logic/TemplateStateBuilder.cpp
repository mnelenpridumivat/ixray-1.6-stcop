#include "stdafx.h"
#include "TemplateStateBuilder.h"
#include "TemplateSection.h"
#include <regex>

bool CTemplateStateBuilder::IsTemplateSection(LPCSTR SectionName)
{
	static std::regex Template(R"((\w+|\w+@\w+)<(\w|:)+>)");
	return std::regex_match(xr_string(SectionName), Template);
}

void CTemplateStateBuilder::PreprocessFile(CInifile* Ltx)
{
	TemplateSections.clear();
	for (const auto& Section : Ltx->sections()) {
		if (IsTemplateSection(*Section->Name)) {
			std::regex SectionNameRegex(R"(/<((\w+)\:?)+>)");
			std::smatch sm;
			xr_string SectNameStr(*Section->Name);
			VERIFY(std::regex_search(SectNameStr, sm, SectionNameRegex));
			TemplateSections.emplace(sm.prefix().str().data(), xr_make_unique<Template>(Section));
		}
	}
}

CState* CTemplateStateBuilder::CreateState(CInifile* Ltx, LPCSTR StateName, xr_vector<shared_str>& NextStates)
{
	return nullptr;
}
