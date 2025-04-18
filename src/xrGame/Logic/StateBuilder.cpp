#include "stdafx.h"
#include "StateBuilder.h"

#include "Transitions/ComplexTransition.h"

using namespace Logic;

CTransition* CStateBuilder::CreateTransition(const xr_string MainCond, xr_string SecondaryConds,
    const xr_string& NextState, xr_string Actions)
{
    static xr_hash_map<shared_str, xr_delegate<CTransition*()>> transitions = {
        {"on_info", &CComplexTransition::Create}
    };
    R_ASSERT3(transitions.contains(MainCond.c_str()), "Unable to find main condition", MainCond.c_str());
    auto Trans = (CComplexTransition*)transitions[MainCond.c_str()]();
    Trans->SetNextState(NextState.c_str());
    static std::regex SecondaryConditionsRegex(R"( *([\-\+\~\=\!][^\-\+\~\=\!\s]+) *)");
    for (std::smatch sm; std::regex_search(SecondaryConds, sm, SecondaryConditionsRegex);)
    {
        xr_string CondStr((sm.str().data()));
        VERIFY(CondStr.size());
        shared_str type;
        {
            string16 buff = {};
            buff[0] = CondStr[0];
            type = buff;
        }
        
        SecondaryConds = sm.suffix().str().data();
    }
    return Trans;
}
