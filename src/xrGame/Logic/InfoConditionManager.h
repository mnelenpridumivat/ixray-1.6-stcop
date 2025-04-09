#pragma once
#include "ConditionManager.h"

class CCondition;

class CInfoConditionManager :
    public CConditionManager
{
    CInfoConditionManager() = default;
    ~CInfoConditionManager() = default;

    xr_hash_map<shared_str, xr_unique_ptr<CCondition>> ConditionsHas;
    xr_hash_map<shared_str, xr_unique_ptr<CCondition>> ConditionsDontHas;

public:
    static CInfoConditionManager& GetInstance();

    CCondition* CreateCondition(shared_str Info, bool Has);

    void OnGiveInfo(shared_str Info);
    void OnDisableInfo(shared_str Info);

    CInfoConditionManager(const CInfoConditionManager& other) = delete;
    CInfoConditionManager(CInfoConditionManager&& other) = delete;

    CInfoConditionManager& operator=(const CInfoConditionManager& other) = delete;
    CInfoConditionManager& operator=(CInfoConditionManager&& other) = delete;

};

