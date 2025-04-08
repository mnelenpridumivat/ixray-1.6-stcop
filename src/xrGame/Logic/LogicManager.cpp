#include "LogicManager.h"

#include <discord_gamesdk/cpp/types.h>

#include "actor_memory.h"
#include "alife_time_manager.h"
#include "visual_memory_manager.h"

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

void CLogicManager::SBinderConditionBase::Execute(MessageBase* data)
{
    VERIFY(Record);
    Record->VerifyConditions();
}

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

void CLogicManager::SBinderConditionFarEnoughNVis::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    auto Casted = Obj->cast_entity_alive();
    R_ASSERT3(Casted, "Can't cast to entity alive", Obj->cName().c_str());
    auto actor = Actor();
    VERIFY(actor);
    Satisfied = Casted->g_Alive() && actor->memory().visual().visible_now(Casted) && Obj->Position().distance_to_sqr(Actor()->Position()) > distance*distance;
}

void CLogicManager::SBinderConditionCloseEnoughNVis::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    auto Casted = Obj->cast_entity_alive();
    R_ASSERT3(Casted, "Can't cast to entity alive", Obj->cName().c_str());
    auto actor = Actor();
    VERIFY(actor);
    Satisfied = Casted->g_Alive() && actor->memory().visual().visible_now(Casted) && Obj->Position().distance_to_sqr(Actor()->Position()) <= distance*distance;
}

void CLogicManager::SBinderConditionFarEnough::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    Satisfied = Obj->Position().distance_to_sqr(Actor()->Position()) > distance*distance;
}

void CLogicManager::SBinderConditionTimer::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Message = (TimerMessage*)data;
    timeLeft = timeLeft < -1 ? -1 : timeLeft - Message->timeDelta;
    Satisfied = timeLeft < 0;
}

void CLogicManager::SBinderConditionNPCInZone::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Message = (NPCZoneMessage*)data;
    Satisfied = Message->Zone == Zone ? Message->Inside : Satisfied;
}

void CLogicManager::SBinderConditionNPCOutZone::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Message = (NPCZoneMessage*)data;
    Satisfied = Message->Zone == Zone ? !Message->Inside : Satisfied;
}

void CLogicManager::SBinderObjectRecord::VerifyConditions()
{
    bool Satisfied = true;
    for (auto elem : Conditions)
    {
        if(!elem->Satisfied)
        {
            Satisfied = false;
            break;
        }
    }
    if(Satisfied)
    {
        auto& manager = CLogicManager::GetInstance();
        for(auto& elem : manager.Binders)
        {
            auto Conds = elem.second.CondsPerObj.find(id);
            VERIFY(Conds != std::end(elem.second.CondsPerObj));
            for(auto& Cond : Conds->second)
            {
                xr_delete(Cond);
            }
            elem.second.CondsPerObj.erase(id);
        }
        {
            xrCriticalSectionGuard guard(manager.ProcessLock);
            manager.ToProcess.emplace(this);
        }
        manager.Records.erase(this);
    }
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
    xrCriticalSectionGuard guard(AquireInfoLock);
    AquiredInfos.push_back({ Info });
}

void CLogicManager::OnReleaseInfo(shared_str Info)
{
    xrCriticalSectionGuard guard(ReleasedInfoLock);
    ReleasedInfos.push_back({ Info });
}

void CLogicManager::OnSignal(ALife::_OBJECT_ID id, shared_str Signal)
{
    xrCriticalSectionGuard guard(SignalLock);
    HappenedSignals.push_back({ id, Signal });
}

void CLogicManager::OnNPCInZone(ALife::_OBJECT_ID id, shared_str ZoneName)
{
    xrCriticalSectionGuard guard(NPCInZoneLock);
    NPCsInZone.push_back({ id, ZoneName });
}

void CLogicManager::OnNPCOutZone(ALife::_OBJECT_ID id, shared_str ZoneName)
{
    xrCriticalSectionGuard guard(NPCOutZoneLock);
    NPCsOutZone.push_back({ id, ZoneName });
}

void CLogicManager::Update()
{
    {
        xrCriticalSectionGuard guard1(ProcessLock);
        xrCriticalSectionGuard guard2(RemoveLock);
        for(auto& elem : ToProcess)
        {
            luabind::functor<void> funct;
            if (ai().script_engine().functor(elem->Callback.c_str(), funct)) {
                funct(elem->id, elem->NextSection.c_str());
            } else
            {
                R_ASSERT4(false, "Unable to process callback", elem->Callback.c_str(), elem->NextSection.c_str());
            }
            ToRemove.emplace(elem);
        }
        ToProcess.clear();
    }
}

void CLogicManager::Update2()
{
    {
        auto& Conds = Binders[EBinderNames::on_actor_dist_le];
        SBinderConditionCloseEnough::MessageBase Message;
        for(auto& elem : Conds.CondsPerObj)
        {
            Message.id = elem.first;
            for(auto& cond : elem.second)
            {
                cond->Execute(&Message);
            }
        }
    }
    {
        auto& Conds = Binders[EBinderNames::on_actor_dist_le_nvis];
        SBinderConditionCloseEnough::MessageBase Message;
        for(auto& elem : Conds.CondsPerObj)
        {
            Message.id = elem.first;
            for(auto& cond : elem.second)
            {
                cond->Execute(&Message);
            }
        }
    }
    {
        auto& Conds = Binders[EBinderNames::on_actor_dist_ge_nvis];
        SBinderConditionCloseEnough::MessageBase Message;
        for(auto& elem : Conds.CondsPerObj)
        {
            Message.id = elem.first;
            for(auto& cond : elem.second)
            {
                cond->Execute(&Message);
            }
        }
    }
    {
        using namespace std::chrono;
        static steady_clock::time_point t1 = steady_clock::now();
        steady_clock::time_point t2 = steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        
        auto& Conds = Binders[EBinderNames::on_timer];
        SBinderConditionTimer::TimerMessage Message;
        Message.timeDelta = elapsed;
        for(auto& elem : Conds.CondsPerObj)
        {
            Message.id = elem.first;
            for(auto& cond : elem.second)
            {
                cond->Execute(&Message);
            }
        }
        t1 = t2;
    }
    {
        static auto t1 = ai().alife().time_manager().game_time();
        auto t2 = ai().alife().time_manager().game_time();
        auto elapsed = t2 - t1;
        auto& Conds = Binders[EBinderNames::on_game_timer];
        SBinderConditionTimer::TimerMessage Message;
        Message.timeDelta = elapsed;
        for(auto& elem : Conds.CondsPerObj)
        {
            Message.id = elem.first;
            for(auto& cond : elem.second)
            {
                cond->Execute(&Message);
            }
        }
        t1 = t2;
    }
    {
        xrCriticalSectionGuard guard1(AquireInfoLock);
        auto& HaveInfoConds = Binders[EBinderNames::on_info_aquired];
        auto& DontHaveInfoConds = Binders[EBinderNames::on_info_lost];
        for(auto& Info : AquiredInfos)
        {
            for(auto& elem : HaveInfoConds.CondsPerObj)
            {
                SBinderConditionInfo::GetInfoMessage Message;
                Message.id = elem.first;
                Message.InfoName = Info.Info;
                Message.IsGet = true;
                for(auto& cond : elem.second)
                {
                    cond->Execute(&Message);
                }
            }
            for(auto& elem : DontHaveInfoConds.CondsPerObj)
            {
                SBinderConditionInfo::GetInfoMessage Message;
                Message.id = elem.first;
                Message.InfoName = Info.Info;
                Message.IsGet = true;
                for(auto& cond : elem.second)
                {
                    cond->Execute(&Message);
                }
            }
        }
    }
    {
        auto& HaveInfoConds = Binders[EBinderNames::on_info_aquired];
        auto& DontHaveInfoConds = Binders[EBinderNames::on_info_lost];
        SBinderConditionInfo::GetInfoMessage Message;
        Message.InfoName = Info;
        Message.IsGet = false;
        for(auto& elem : HaveInfoConds.CondsPerObj)
        {
            for(auto& cond : elem.second)
            {
                cond->Execute(&Message);
            }
        }
        for(auto& elem : DontHaveInfoConds.CondsPerObj)
        {
            for(auto& cond : elem.second)
            {
                cond->Execute(&Message);
            }
        }
    }
    {
        auto& Conds = Binders[EBinderNames::on_signal];
        auto CondRow = Conds.CondsPerObj.find(id);
        if(CondRow != Conds.CondsPerObj.end())
        {
            SBinderConditionSignal::SignalMessage Message;
            Message.id = id;
            Message.Signal = Signal;
            for(auto& elem : CondRow->second)
            {
                elem->Execute(&Message);
            }
        }
    }
    {
        SBinderConditionNPCZone::NPCZoneMessage Message;
        Message.id = id;
        Message.Zone = ZoneName;
        Message.Inside = true;
        {
            auto& Conds = Binders[EBinderNames::on_npc_in_zone];
            auto CondRow = Conds.CondsPerObj.find(id);
            if(CondRow != Conds.CondsPerObj.end())
            {
                for(auto& elem : CondRow->second)
                {
                    elem->Execute(&Message);
                }
            }
        }
        {
            auto& Conds = Binders[EBinderNames::on_npc_not_in_zone];
            auto CondRow = Conds.CondsPerObj.find(id);
            if(CondRow != Conds.CondsPerObj.end())
            {
                for(auto& elem : CondRow->second)
                {
                    elem->Execute(&Message);
                }
            }
        }
        if(id == Actor()->ID())
        {
            {
                auto& Conds = Binders[EBinderNames::on_actor_in_zone];
                auto CondRow = Conds.CondsPerObj.find(id);
                if(CondRow != Conds.CondsPerObj.end())
                {
                    for(auto& elem : CondRow->second)
                    {
                        elem->Execute(&Message);
                    }
                }
            }
            {
                auto& Conds = Binders[EBinderNames::on_actor_not_in_zone];
                auto CondRow = Conds.CondsPerObj.find(id);
                if(CondRow != Conds.CondsPerObj.end())
                {
                    for(auto& elem : CondRow->second)
                    {
                        elem->Execute(&Message);
                    }
                }
            }
        }
    }
    {
        SBinderConditionNPCZone::NPCZoneMessage Message;
        Message.id = id;
        Message.Zone = ZoneName;
        Message.Inside = false;
        {
            auto& Conds = Binders[EBinderNames::on_npc_in_zone];
            auto CondRow = Conds.CondsPerObj.find(id);
            if(CondRow != Conds.CondsPerObj.end())
            {
                for(auto& elem : CondRow->second)
                {
                    elem->Execute(&Message);
                }
            }
        }
        {
            auto& Conds = Binders[EBinderNames::on_npc_not_in_zone];
            auto CondRow = Conds.CondsPerObj.find(id);
            if(CondRow != Conds.CondsPerObj.end())
            {
                for(auto& elem : CondRow->second)
                {
                    elem->Execute(&Message);
                }
            }
        }
    }
    {
        xrCriticalSectionGuard guard(RemoveLock);
        for(auto& elem : ToRemove)
        {
            xr_delete(elem);
        }
        ToRemove.clear();
    }
}
