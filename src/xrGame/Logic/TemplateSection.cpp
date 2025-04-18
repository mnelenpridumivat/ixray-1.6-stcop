#include "stdafx.h"
#include "TemplateSection.h"
#include <regex>

using namespace Logic;

Template::Template(CInifile::Sect* Sect)
{
    {
        std::regex SectionNameRegex(R"(/<((\w+)\:?)+>/g)");
        std::smatch sm;
        xr_string SectNameStr(*Sect->Name);
        VERIFY(std::regex_search(SectNameStr, sm, SectionNameRegex));
        xr_string SectionNameTemplates(sm.str().data());
        std::regex r(R"((\w+)\:?)");
        for (std::smatch sm; std::regex_search(SectionNameTemplates, sm, r);)
        {
            xr_string ParamName((sm.str().data()));
            if ((*ParamName.crbegin()) == ':') {
                ParamName.pop_back();
            }
            TemplateParams.push_back(shared_str(ParamName.c_str()));
            SectionNameTemplates = sm.suffix().str().data();
        }
    }
    for (const auto& Field : Sect->Data) {
        auto It = Params.emplace({});
        It->Key = Field.first;

        xr_string FieldValueFull(*Field.second);
        xr_string FieldValueBegin;
        std::regex r(R"((\w+)\:?)");
        for (std::smatch sm; std::regex_search(FieldValueFull, sm, r);)
        {
            if (!FieldValueBegin.empty()) {
                xr_string str = FieldValueFull.substr(0, FieldValueFull.size() - FieldValueFull.find(FieldValueBegin));
                It->Value.push_back(str);
            }
            xr_string ParamName((sm.str().data()));
            shared_str ParamNameStr(ParamName.substr(1, ParamName.size() - 1).c_str());

            for (size_t i = 0; i < TemplateParams.size(); ++i) {
                if (TemplateParams[i] == ParamNameStr) {
                    It->Value.push_back(i);
                    break;
                }
            }

            FieldValueBegin = sm.suffix().str().data();
        }
        if (!FieldValueBegin.empty()) {
            It->Value.push_back(FieldValueBegin);
        }
    }
}
