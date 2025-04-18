#include "stdafx.h"
#include "ActualStateBuilder.h"

#include "LogicManager.h"
#include "State.h"

using namespace Logic;

void CActualStateBuilder::PreprocessFile(CInifile* Ltx)
{
}

xr_unique_ptr<Logic::CState>&& CActualStateBuilder::CreateState(CInifile* Ltx, shared_str StateName, xr_deque<shared_str>& NextStates)
{
	VERIFY(Ltx);
	R_ASSERT4(Ltx->section_exist(StateName), "There is not required state in file", StateName.c_str(), Ltx->fname());
	CInifile::Sect& sect = Ltx->r_section(StateName);
	xr_unique_ptr<CState> RetVal = xr_make_unique<CState>();
	RetVal->StateName = StateName;
	for (auto elem : sect.Data)
	{
		if (CLogicManager::GetInstance().CanHandle(elem.first.c_str()))
		{
			static std::regex CondSep(R"( *([^,]+) *)");
			xr_string str = elem.second.c_str();
			for (std::smatch sm; std::regex_search(str, sm, CondSep);)
			{
				xr_string Conditions = "", Actions = "", NextSection = "";
				xr_string CondStr((sm.str().data()));
				{
					static std::regex CondRegex(R"({ *(.*) *})");
					std::smatch sm2;
					if(std::regex_search(str, sm2, CondRegex))
					{
						Conditions = sm2.str().data();
						str = xr_string(sm2.prefix().str().data())
								+ xr_string(sm2.suffix().str().data());
					}
				}
				{
					static std::regex ActionRegex(R"(% *(.*) *%)");
					std::smatch sm2;
					if(std::regex_search(str, sm2, ActionRegex))
					{
						Actions = sm2.str().data();
						str = xr_string(sm2.prefix().str().data())
								+ xr_string(sm2.suffix().str().data());
					}
				}
				{
					static std::regex SectionRegex(R"( *(.*) *)");
					std::smatch sm2;
					if(std::regex_search(str, sm2, SectionRegex))
					{
						NextSection = sm2.str().data();
						str = xr_string(sm2.prefix().str().data())
								+ xr_string(sm2.suffix().str().data());
					}
				}
				RetVal->Transitions.emplace_back(CreateTransition(elem.first.c_str(), Conditions, NextSection, Actions));
				NextStates.push_back(NextSection.c_str());
				str = sm.suffix().str().data();
			}
			
		} else
		{
			xr_string str = elem.second.c_str();
			size_t npos = str.find_first_not_of (".+-0123456789");
			if ( npos == std::string::npos ) {
				RetVal->Mappings[elem.first] = std::stof (str.c_str());
			} else
			{
				RetVal->Mappings[elem.first] = str.c_str();
			}
		}
	}
	return std::move(RetVal);
}
