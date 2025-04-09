#include "stdafx.h"
#include "InfoConditionManager.h"
#include "Condition.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_graph_registry.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_registry_container.h"
#include "alife_registry_container_composition.h"

CInfoConditionManager& CInfoConditionManager::GetInstance()
{
    static CInfoConditionManager instance;
    return instance;
}

CCondition* CInfoConditionManager::CreateCondition(shared_str Info, bool Has)
{
    xr_hash_map<shared_str, xr_unique_ptr<CCondition>>* RequiedMap = nullptr;
    if (Has) {
        RequiedMap = &ConditionsHas;
    }
    else {
        RequiedMap = &ConditionsDontHas;
    }
    auto ActorID = ai().get_alife()->graph().actor()->ID;
    auto Condition = RequiedMap->find(Info);
    if (Condition != RequiedMap->end()) {
        return Condition->second.get();
    }
    auto known_info = ai().get_alife()->registry<CInfoPortionRegistry>(nullptr).object(ActorID, true);
    bool HasInfo = true;
    if (std::find_if(known_info->begin(), known_info->end(), CFindByIDPred(Info)) == known_info->end()) {
        HasInfo = false;
    }
    return RequiedMap->emplace(Info, xr_make_unique<CCondition>(HasInfo)).first->second.get();
}

void CInfoConditionManager::OnGiveInfo(shared_str Info)
{
    auto Condition = ConditionsDontHas.find(Info);
    if (Condition != ConditionsDontHas.end()) {
        Condition->second->SetIsSucced(false);
    }
    Condition = ConditionsHas.find(Info);
    if (Condition != ConditionsHas.end()) {
        Condition->second->SetIsSucced(true);
    }
}

void CInfoConditionManager::OnDisableInfo(shared_str Info)
{
    auto Condition = ConditionsHas.find(Info);
    if (Condition != ConditionsHas.end()) {
        Condition->second->SetIsSucced(false);
    }
    Condition = ConditionsDontHas.find(Info);
    if (Condition != ConditionsDontHas.end()) {
        Condition->second->SetIsSucced(true);
    }
}
