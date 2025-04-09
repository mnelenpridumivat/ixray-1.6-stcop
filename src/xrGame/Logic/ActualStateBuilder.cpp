#include "stdafx.h"
#include "ActualStateBuilder.h"

#include "LogicManager.h"
#include "State.h"

void CActualStateBuilder::PreprocessFile(CInifile* Ltx)
{
}

xr_unique_ptr<CState>&& CActualStateBuilder::CreateState(CInifile* Ltx, shared_str StateName, xr_deque<shared_str>& NextStates)
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
