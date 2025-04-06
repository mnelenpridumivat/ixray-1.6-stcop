#include "LogicManager.h"

const char EBinderNames::on_actor_dist_le[] = "on_actor_dist_le";
const char EBinderNames::on_actor_dist_le_nvis[] = "on_actor_dist_le_nvis";
const char EBinderNames::on_actor_dist_ge[] = "on_actor_dist_ge";
const char EBinderNames::on_actor_dist_ge_nvis[] = "on_actor_dist_ge_nvis";
const char EBinderNames::on_signal[] = "on_signal";
const char EBinderNames::on_timer[] = "on_timer";
const char EBinderNames::on_game_timer[] = "on_game_timer";
const char EBinderNames::on_actor_in_zone[] = "on_actor_in_zone";
const char EBinderNames::on_actor_not_in_zone[] = "on_actor_not_in_zone";
const char EBinderNames::on_npc_in_zone[] = "on_npc_in_zone";
const char EBinderNames::on_npc_not_in_zone[] = "on_npc_not_in_zone";
const char EBinderNames::on_actor_inside[] = "on_actor_inside";
const char EBinderNames::on_actor_outside[] = "on_actor_outside";
const char EBinderNames::on_info_aquired[] = "on_info_aquired";
const char EBinderNames::on_info_lost[] = "on_info_lost";

void CLogicManager::SBinderConditionHasInfo::Execute(MessageBase* data)
{
    VERIFY(data);
    GetInfoMessage* message = (GetInfoMessage*)data;
    if(message->InfoName == InfoName)
    {
        Satisfied = message->IsGet;
    }
}

void CLogicManager::SBinderConditionDontHasInfo::Execute(MessageBase* data)
{
    VERIFY(data);
    GetInfoMessage* message = (GetInfoMessage*)data;
    if(message->InfoName == InfoName)
    {
        Satisfied = !message->IsGet;
    }
}

void CLogicManager::SBinderConditionCloseEnough::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    Satisfied = Obj->Position().distance_to_sqr(Actor()->Position()) <= distance*distance;
}

void CLogicManager::SBinderConditionFarEnough::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    Satisfied = Obj->Position().distance_to_sqr(Actor()->Position()) > distance*distance;
}

CLogicManager::CLogicManager()
{
    Binders[EBinderNames::on_actor_dist_le] = {};
    Binders[EBinderNames::on_actor_dist_le_nvis] = {};
    Binders[EBinderNames::on_actor_dist_ge] = {};
    Binders[EBinderNames::on_actor_dist_ge_nvis] = {};
    Binders[EBinderNames::on_signal] = {};
    Binders[EBinderNames::on_timer] = {};
    Binders[EBinderNames::on_game_timer] = {};
    Binders[EBinderNames::on_actor_in_zone] = {};
    Binders[EBinderNames::on_actor_not_in_zone] = {};
    Binders[EBinderNames::on_npc_in_zone] = {};
    Binders[EBinderNames::on_npc_not_in_zone] = {};
    Binders[EBinderNames::on_actor_inside] = {};
    Binders[EBinderNames::on_actor_outside] = {};
    Binders[EBinderNames::on_info_aquired] = {};
    Binders[EBinderNames::on_info_lost] = {};
}

CLogicManager& CLogicManager::GetInstance()
{
    static CLogicManager instance;
    return instance;
}

void CLogicManager::OnAquireInfo(shared_str Info)
{
    auto& HaveInfoConds = Binders[EBinderNames::on_info_aquired];
    auto& DontHaveInfoConds = Binders[EBinderNames::on_info_lost];
    for(auto& elem : HaveInfoConds.Binders)
    {
        SBinderConditionInfo::GetInfoMessage Message;
        Message.id = elem.first;
        Message.InfoName = Info;
        Message.IsGet = true;
        for(auto& cond : elem.second)
        {
            cond->Execute(&Message);
        }
    }
    for(auto& elem : DontHaveInfoConds.Binders)
    {
        SBinderConditionInfo::GetInfoMessage Message;
        Message.id = elem.first;
        Message.InfoName = Info;
        Message.IsGet = true;
        for(auto& cond : elem.second)
        {
            cond->Execute(&Message);
        }
    }
}

void CLogicManager::OnReleaseInfo(shared_str Info)
{
    auto& HaveInfoConds = Binders[EBinderNames::on_info_aquired];
    auto& DontHaveInfoConds = Binders[EBinderNames::on_info_lost];
    for(auto& elem : HaveInfoConds.Binders)
    {
        SBinderConditionInfo::GetInfoMessage Message;
        Message.InfoName = Info;
        Message.IsGet = false;
        for(auto& cond : elem.second)
        {
            cond->Execute(&Message);
        }
    }
    for(auto& elem : DontHaveInfoConds.Binders)
    {
        SBinderConditionInfo::GetInfoMessage Message;
        Message.InfoName = Info;
        Message.IsGet = false;
        for(auto& cond : elem.second)
        {
            cond->Execute(&Message);
        }
    }
}

void CLogicManager::Update()
{
    {
        auto& Conds = Binders[EBinderNames::on_actor_dist_le_nvis];
        for(auto& elem : Conds.Binders)
        {
            elem->Execute(nullptr);
        }
    }
    {
        auto& Conds = Binders[EBinderNames::on_actor_dist_ge_nvis];
        for(auto& elem : Conds.Binders)
        {
            elem->Execute(nullptr);
        }
    }
}
