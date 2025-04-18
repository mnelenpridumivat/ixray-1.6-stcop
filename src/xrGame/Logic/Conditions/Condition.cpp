#include "stdafx.h"
#include "Condition.h"

using namespace Logic;

void CCondition::OnConditionActivate()
{
}

void CCondition::OnConditionDeactivate()
{
}

CCondition* CCondition::CreateCondition()
{
    static xr_hash_map<shared_str, xr_delegate<CCondition*()>> conditions = {
        {"+", },
        {"-", }
    };
}
