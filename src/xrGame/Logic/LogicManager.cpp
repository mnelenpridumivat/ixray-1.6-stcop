#include "stdafx.h"
#include "LogicManager.h"

#include <discord_gamesdk/cpp/types.h>

#include "actor_memory.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "visual_memory_manager.h"

using namespace luabind;

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
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionDontHasInfo::Execute(MessageBase* data)
{
    VERIFY(data);
    GetInfoMessage* message = (GetInfoMessage*)data;
    if(message->InfoName == InfoName)
    {
        Satisfied = !message->IsGet;
    }
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionCloseEnoughNVis::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    Satisfied = Obj->Position().distance_to_sqr(Actor()->Position()) <= distance*distance;
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionFarEnough::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    auto Casted = Obj->cast_entity_alive();
    R_ASSERT3(Casted, "Can't cast to entity alive", Obj->cName().c_str());
    auto actor = Actor();
    VERIFY(actor);
    Satisfied = Casted->g_Alive() && actor->memory().visual().visible_now(Casted) && Obj->Position().distance_to_sqr(Actor()->Position()) > distance*distance;
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionCloseEnough::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    auto Casted = Obj->cast_entity_alive();
    R_ASSERT3(Casted, "Can't cast to entity alive", Obj->cName().c_str());
    auto actor = Actor();
    VERIFY(actor);
    Satisfied = Casted->g_Alive() && actor->memory().visual().visible_now(Casted) && Obj->Position().distance_to_sqr(Actor()->Position()) <= distance*distance;
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionFarEnoughNVis::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Obj = Level().Objects.net_Find(data->id);
    VERIFY(Obj);
    Satisfied = Obj->Position().distance_to_sqr(Actor()->Position()) > distance*distance;
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionTimer::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Message = (TimerMessage*)data;
    timeLeft = timeLeft < -1 ? -1 : timeLeft - Message->timeDelta;
    Satisfied = timeLeft < 0;
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionSignal::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Message = (SignalMessage*)data;
    Satisfied = Message->Signal == Signal;
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionNPCInZone::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Message = (NPCZoneMessage*)data;
    Satisfied = Message->Zone == Zone && Message->id == npc_id ? Message->Inside : Satisfied;
    SBinderConditionBase::Execute(data);
}

void CLogicManager::SBinderConditionNPCOutZone::Execute(MessageBase* data)
{
    VERIFY(data);
    auto Message = (NPCZoneMessage*)data;
    Satisfied = Message->Zone == Zone && Message->id == npc_id ? !Message->Inside : Satisfied;
    SBinderConditionBase::Execute(data);
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

void CLogicManager::Serialize(ISaveObject& Object)
{
    /*BEGIN_CHUNK(Object, "CLogicManager")
    {
        xrCriticalSectionGuard g1(ProcessLock);
        xrCriticalSectionGuard g2(AquireInfoLock);
        xrCriticalSectionGuard g3(ReleasedInfoLock);
        xrCriticalSectionGuard g4(SignalLock);
        xrCriticalSectionGuard g5(NPCInZoneLock);
        xrCriticalSectionGuard g6(NPCOutZoneLock);
        xr_vector<xrCriticalSectionGuard> gs;
        for(auto& elem : Binders)
        {
            gs.emplace_back(elem.second.lock);
        }
        BEGIN_CHUNK(Object, "CLogicManager::Records")
        {
            Object << Records;
        }
    }*/
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

void CLogicManager::OnNPCInZone(ALife::_OBJECT_ID id, ALife::_OBJECT_ID npc_id, shared_str ZoneName)
{
    xrCriticalSectionGuard guard(NPCInZoneLock);
    NPCsInZone.push_back({ id, npc_id, ZoneName });
}

void CLogicManager::OnNPCOutZone(ALife::_OBJECT_ID id, ALife::_OBJECT_ID npc_id, shared_str ZoneName)
{
    xrCriticalSectionGuard guard(NPCOutZoneLock);
    NPCsOutZone.push_back({ id, npc_id, ZoneName });
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
        xrCriticalSectionGuard guard(Conds.lock);
        SBinderConditionCloseEnoughNVis::MessageBase Message;
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
        xrCriticalSectionGuard guard(Conds.lock);
        SBinderConditionCloseEnoughNVis::MessageBase Message;
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
        xrCriticalSectionGuard guard(Conds.lock);
        SBinderConditionCloseEnoughNVis::MessageBase Message;
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
        auto& Conds = Binders[EBinderNames::on_timer];
        xrCriticalSectionGuard guard(Conds.lock);
        using namespace std::chrono;
        static steady_clock::time_point t1 = steady_clock::now();
        steady_clock::time_point t2 = steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
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
        auto& Conds = Binders[EBinderNames::on_game_timer];
        xrCriticalSectionGuard guard(Conds.lock);
        static auto t1 = ai().alife().time_manager().game_time();
        auto t2 = ai().alife().time_manager().game_time();
        auto elapsed = t2 - t1;
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
        xrCriticalSectionGuard guard2(HaveInfoConds.lock);
        auto& DontHaveInfoConds = Binders[EBinderNames::on_info_lost];
        xrCriticalSectionGuard guard3(DontHaveInfoConds.lock);
        SBinderConditionInfo::GetInfoMessage Message;
        Message.IsGet = true;
        for(auto& Info : AquiredInfos)
        {
            Message.InfoName = Info.Info;
            for(auto& elem : HaveInfoConds.CondsPerObj)
            {
                Message.id = elem.first;
                for(auto& cond : elem.second)
                {
                    cond->Execute(&Message);
                }
            }
            for(auto& elem : DontHaveInfoConds.CondsPerObj)
            {
                Message.id = elem.first;
                for(auto& cond : elem.second)
                {
                    cond->Execute(&Message);
                }
            }
        }
    }
    {
        xrCriticalSectionGuard guard1(ReleasedInfoLock);
        auto& HaveInfoConds = Binders[EBinderNames::on_info_aquired];
        xrCriticalSectionGuard guard2(HaveInfoConds.lock);
        auto& DontHaveInfoConds = Binders[EBinderNames::on_info_lost];
        xrCriticalSectionGuard guard3(DontHaveInfoConds.lock);
        SBinderConditionInfo::GetInfoMessage Message;
        Message.IsGet = false;
        for(auto& Info : ReleasedInfos)
        {
            Message.InfoName = Info.Info;
            for(auto& elem : HaveInfoConds.CondsPerObj)
            {
                Message.id = elem.first;
                for(auto& cond : elem.second)
                {
                    cond->Execute(&Message);
                }
            }
            for(auto& elem : DontHaveInfoConds.CondsPerObj)
            {
                Message.id = elem.first;
                for(auto& cond : elem.second)
                {
                    cond->Execute(&Message);
                }
            }
        }
    }
    {
        xrCriticalSectionGuard guard1(SignalLock);
        auto& Conds = Binders[EBinderNames::on_signal];
        xrCriticalSectionGuard guard2(Conds.lock);
        SBinderConditionSignal::SignalMessage Message;
        for(auto& signal : HappenedSignals)
        {
            auto CondRow = Conds.CondsPerObj.find(signal.id);
            if(CondRow != Conds.CondsPerObj.end())
            {
                Message.id = signal.id;
                Message.Signal = signal.Signal;
                for(auto& elem : CondRow->second)
                {
                    elem->Execute(&Message);
                }
            }
        }
    }
    {
        xrCriticalSectionGuard guard1(NPCInZoneLock);
        SBinderConditionNPCZone::NPCZoneMessage Message;
        Message.Inside = true;
        for(auto& event : NPCsInZone)
        {
            {
                auto& Conds = Binders[EBinderNames::on_npc_in_zone];
                xrCriticalSectionGuard guard2(Conds.lock);
                auto CondRow = Conds.CondsPerObj.find(event.id);
                if(CondRow != Conds.CondsPerObj.end())
                {
                    Message.id = event.npc_id;
                    Message.Zone = event.Zone;
                    for(auto& elem : CondRow->second)
                    {
                        elem->Execute(&Message);
                    }
                }
            }
            {
                auto& Conds = Binders[EBinderNames::on_npc_not_in_zone];
                xrCriticalSectionGuard guard2(Conds.lock);
                auto CondRow = Conds.CondsPerObj.find(event.id);
                if(CondRow != Conds.CondsPerObj.end())
                {
                    Message.id = event.npc_id;
                    Message.Zone = event.Zone;
                    for(auto& elem : CondRow->second)
                    {
                        elem->Execute(&Message);
                    }
                }
            }
            if(event.id == Actor()->ID())
            {
                {
                    auto& Conds = Binders[EBinderNames::on_actor_in_zone];
                    xrCriticalSectionGuard guard2(Conds.lock);
                    auto CondRow = Conds.CondsPerObj.find(event.id);
                    if(CondRow != Conds.CondsPerObj.end())
                    {
                        Message.id = event.npc_id;
                        Message.Zone = event.Zone;
                        for(auto& elem : CondRow->second)
                        {
                            elem->Execute(&Message);
                        }
                    }
                }
                {
                    auto& Conds = Binders[EBinderNames::on_actor_not_in_zone];
                    xrCriticalSectionGuard guard2(Conds.lock);
                    auto CondRow = Conds.CondsPerObj.find(event.id);
                    if(CondRow != Conds.CondsPerObj.end())
                    {
                        Message.id = event.npc_id;
                        Message.Zone = event.Zone;
                        for(auto& elem : CondRow->second)
                        {
                            elem->Execute(&Message);
                        }
                    }
                }
            }
        }
    }
    {
        xrCriticalSectionGuard guard1(NPCOutZoneLock);
        SBinderConditionNPCZone::NPCZoneMessage Message;
        Message.Inside = false;
        for(auto& event : NPCsOutZone)
        {
            {
                auto& Conds = Binders[EBinderNames::on_npc_in_zone];
                xrCriticalSectionGuard guard2(Conds.lock);
                auto CondRow = Conds.CondsPerObj.find(event.id);
                if(CondRow != Conds.CondsPerObj.end())
                {
                    Message.id = event.npc_id;
                    Message.Zone = event.Zone;
                    for(auto& elem : CondRow->second)
                    {
                        elem->Execute(&Message);
                    }
                }
            }
            {
                auto& Conds = Binders[EBinderNames::on_npc_not_in_zone];
                xrCriticalSectionGuard guard2(Conds.lock);
                auto CondRow = Conds.CondsPerObj.find(event.id);
                if(CondRow != Conds.CondsPerObj.end())
                {
                    Message.id = event.npc_id;
                    Message.Zone = event.Zone;
                    for(auto& elem : CondRow->second)
                    {
                        elem->Execute(&Message);
                    }
                }
            }
            if(event.id == Actor()->ID())
            {
                {
                    auto& Conds = Binders[EBinderNames::on_actor_in_zone];
                    xrCriticalSectionGuard guard2(Conds.lock);
                    auto CondRow = Conds.CondsPerObj.find(event.id);
                    if(CondRow != Conds.CondsPerObj.end())
                    {
                        Message.id = event.npc_id;
                        Message.Zone = event.Zone;
                        for(auto& elem : CondRow->second)
                        {
                            elem->Execute(&Message);
                        }
                    }
                }
                {
                    auto& Conds = Binders[EBinderNames::on_actor_not_in_zone];
                    xrCriticalSectionGuard guard2(Conds.lock);
                    auto CondRow = Conds.CondsPerObj.find(event.id);
                    if(CondRow != Conds.CondsPerObj.end())
                    {
                        Message.id = event.npc_id;
                        Message.Zone = event.Zone;
                        for(auto& elem : CondRow->second)
                        {
                            elem->Execute(&Message);
                        }
                    }
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

bool CLogicManager::CanHandle(LPCSTR Cond)
{
    return Binders.contains(Cond);
}

void CLogicManager::BeginConstruction(ALife::_OBJECT_ID id, LPCSTR Callback, LPCSTR NextSection)
{
    R_ASSERT4(ConstructingRecord == nullptr, "There is already a record to construct", Callback, NextSection);
    ConstructingRecord = new SBinderObjectRecord(id, Callback, NextSection);
}

void CLogicManager::EndConstruction()
{
    Records.emplace(ConstructingRecord);
    ConstructingRecord = nullptr;
}

void CLogicManager::BindOnActorDistLe(ALife::_OBJECT_ID id, float distance)
{
    auto NewCond = new SBinderConditionCloseEnough();
    NewCond->distance = distance;
    {
        auto& Cond = Binders[EBinderNames::on_actor_dist_le];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnActorDistLeNVis(ALife::_OBJECT_ID id, float distance)
{
    auto NewCond = new SBinderConditionCloseEnoughNVis();
    NewCond->distance = distance;
    {
        auto& Cond = Binders[EBinderNames::on_actor_dist_le_nvis];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnActorDistGe(ALife::_OBJECT_ID id, float distance)
{
    auto NewCond = new SBinderConditionFarEnough();
    NewCond->distance = distance;
    {
        auto& Cond = Binders[EBinderNames::on_actor_dist_ge];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnActorDistGeNVis(ALife::_OBJECT_ID id, float distance)
{
    auto NewCond = new SBinderConditionFarEnoughNVis();
    NewCond->distance = distance;
    {
        auto& Cond = Binders[EBinderNames::on_actor_dist_ge_nvis];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnSignal(ALife::_OBJECT_ID id, LPCSTR Signal)
{
    auto NewCond = new SBinderConditionSignal();
    NewCond->Signal = Signal;
    {
        auto& Cond = Binders[EBinderNames::on_signal];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnTimer(ALife::_OBJECT_ID id, float time)
{
    auto NewCond = new SBinderConditionTimer();
    NewCond->timeLeft = time;
    {
        auto& Cond = Binders[EBinderNames::on_timer];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnGameTimer(ALife::_OBJECT_ID id, float time)
{
    auto NewCond = new SBinderConditionTimer();
    NewCond->timeLeft = time;
    {
        auto& Cond = Binders[EBinderNames::on_game_timer];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnActorInZone(ALife::_OBJECT_ID id, LPCSTR zone)
{
    auto NewCond = new SBinderConditionNPCInZone();
    NewCond->npc_id = Actor()->ID();
    NewCond->Zone = zone;
    {
        auto& Cond = Binders[EBinderNames::on_npc_in_zone];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnActorNotInZone(ALife::_OBJECT_ID id, LPCSTR zone)
{
    auto NewCond = new SBinderConditionNPCOutZone();
    NewCond->npc_id = Actor()->ID();
    NewCond->Zone = zone;
    {
        auto& Cond = Binders[EBinderNames::on_npc_not_in_zone];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnNpcInZone(ALife::_OBJECT_ID id, ALife::_OBJECT_ID npc_id, LPCSTR zone)
{
    auto NewCond = new SBinderConditionNPCInZone();
    NewCond->npc_id = npc_id;
    NewCond->Zone = zone;
    {
        auto& Cond = Binders[EBinderNames::on_npc_in_zone];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnNpcNotInZone(ALife::_OBJECT_ID id, ALife::_OBJECT_ID npc_id, LPCSTR zone)
{
    auto NewCond = new SBinderConditionNPCOutZone();
    NewCond->npc_id = npc_id;
    NewCond->Zone = zone;
    {
        auto& Cond = Binders[EBinderNames::on_npc_not_in_zone];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnActorInside(ALife::_OBJECT_ID id)
{
    auto NewCond = new SBinderConditionNPCInZone();
    NewCond->npc_id = Actor()->ID();
    auto Obj = Level().Objects.net_Find(id);
    VERIFY(Obj);
    NewCond->Zone = Obj->cName();
    {
        auto& Cond = Binders[EBinderNames::on_npc_in_zone];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnActorOutside(ALife::_OBJECT_ID id)
{
    auto NewCond = new SBinderConditionNPCOutZone();
    NewCond->npc_id = Actor()->ID();
    auto Obj = Level().Objects.net_Find(id);
    VERIFY(Obj);
    NewCond->Zone = Obj->cName();
    {
        auto& Cond = Binders[EBinderNames::on_npc_not_in_zone];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnInfoAquired(ALife::_OBJECT_ID id, LPCSTR Info)
{
    auto NewCond = new SBinderConditionHasInfo();
    NewCond->InfoName = Info;
    {
        auto& Cond = Binders[EBinderNames::on_info_aquired];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

void CLogicManager::BindOnInfoLost(ALife::_OBJECT_ID id, LPCSTR Info)
{
    auto NewCond = new SBinderConditionDontHasInfo();
    NewCond->InfoName = Info;
    {
        auto& Cond = Binders[EBinderNames::on_info_lost];
        xrCriticalSectionGuard guard(Cond.lock);
        if(!Cond.CondsPerObj.contains(id))
        {
            Cond.CondsPerObj[id] = {};
        }
        Cond.CondsPerObj[id].push_back(NewCond);
    }
    ConstructingRecord->Conditions.push_back(NewCond);
    NewCond->Record = ConstructingRecord;
}

CLogicManager* getCLogicManager()
{
    return &CLogicManager::GetInstance();
}

void CLogicManager::script_register(lua_State* L)
{
    module(L)[
        class_<CLogicManager>("CLogicManager")
        .def("can_handle", &CLogicManager::CanHandle)
        .def("begin_construction", &CLogicManager::BeginConstruction)
        .def("end_construction", &CLogicManager::EndConstruction)
        .def("bind_on_actor_dist_le", &CLogicManager::BindOnActorDistLe)
        .def("bind_on_actor_dist_le_nvis", &CLogicManager::BindOnActorDistLeNVis)
        .def("bind_on_actor_dist_ge", &CLogicManager::BindOnActorDistGe)
        .def("bind_on_actor_dist_ge_nvis", &CLogicManager::BindOnActorDistGeNVis)
        .def("bind_on_signal", &CLogicManager::BindOnSignal)
        .def("bind_on_timer", &CLogicManager::BindOnTimer)
        .def("bind_on_game_timer", &CLogicManager::BindOnGameTimer)
        .def("bind_on_actor_in_zone", &CLogicManager::BindOnActorInZone)
        .def("bind_on_actor_not_in_zone", &CLogicManager::BindOnActorNotInZone)
        .def("bind_on_npc_in_zone", &CLogicManager::BindOnNpcInZone)
        .def("bind_on_npc_not_in_zone", &CLogicManager::BindOnNpcNotInZone)
        .def("bind_on_actor_inside", &CLogicManager::BindOnActorInside)
        .def("bind_on_actor_outside", &CLogicManager::BindOnActorOutside)
        .def("bind_on_info_aquired", &CLogicManager::BindOnInfoAquired)
        .def("bind_on_info_lost", &CLogicManager::BindOnInfoLost)
        ,
        def("logic_manager", &getCLogicManager)
    ];
}
