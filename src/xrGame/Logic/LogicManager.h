#pragma once
#include "script_game_object.h"

class ISaveObject;

struct EBinderNames
{
    static const char on_actor_dist_le[];
    static const char on_actor_dist_le_nvis[];
    static const char on_actor_dist_ge[];
    static const char on_actor_dist_ge_nvis[];
    static const char on_signal[];
    static const char on_timer[];
    static const char on_game_timer[];
    static const char on_actor_in_zone[];
    static const char on_actor_not_in_zone[];
    static const char on_npc_in_zone[];
    static const char on_npc_not_in_zone[];
    static const char on_actor_inside[]; // same as on_actor_in_zone
    static const char on_actor_outside[]; // same as on_actor_not_in_zone
    static const char on_info_aquired[];
    static const char on_info_lost[];
};

class CLogicManager
{
    struct SLogicBindRecord;
    
    struct SLogicCondition
    {
        SLogicBindRecord* bindRecord = nullptr;
        bool IsSatisfied = false;
    };
    
    struct SLogicBindRecord
    {
        xr_vector<SLogicCondition> conditions;
        shared_str BinderName;
        
    };

    struct SBinderConditionBase
    {
        struct MessageBase
        {
            ALife::_OBJECT_ID id = ALife::_OBJECT_ID(-1);
        };
        virtual ~SBinderConditionBase() = default;
        bool Satisfied = false;
        virtual void Execute(MessageBase* data) = 0;
    };

    struct SBinderConditionInfo : SBinderConditionBase
    {
        struct GetInfoMessage : MessageBase
        {
            bool IsGet;
            shared_str InfoName;
        };
        shared_str InfoName;
    };

    struct SBinderConditionHasInfo : SBinderConditionInfo
    {
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionDontHasInfo : SBinderConditionInfo
    {
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionDistance : SBinderConditionBase
    {
        float distance;
    };

    struct SBinderConditionCloseEnough : SBinderConditionDistance
    {
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionFarEnough : SBinderConditionDistance
    {
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionCloseEnoughNVis : SBinderConditionDistance
    {
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionFarEnoughNVis : SBinderConditionDistance
    {
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionTimer : SBinderConditionBase
    {
        struct TimerMessage : MessageBase
        {
            float timeDelta;
        };
        float timeLeft;
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionSignal: SBinderConditionBase
    {
        struct SignalMessage : MessageBase
        {
            shared_str Signal;
        };
        shared_str Signal;
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionNPCZone: SBinderConditionBase
    {
        struct NPCZoneMessage : MessageBase
        {
            bool Inside;
            shared_str Zone;
        };
        shared_str Zone;
    };

    struct SBinderConditionNPCInZone: SBinderConditionNPCZone
    {
        void Execute(MessageBase* data) override;
    };

    struct SBinderConditionNPCOutZone: SBinderConditionNPCZone
    {
        void Execute(MessageBase* data) override;
    };

    struct SBindersRow
    {
        xr_hash_map<ALife::_OBJECT_ID, xr_vector<SBinderConditionBase*>> Binders;
    };

    struct SPendingRemoveBinder
    {
        ALife::_OBJECT_ID objectID;
        shared_str BinderType;
    };

    xr_hash_map<shared_str,SBindersRow> Binders;
    xr_vector<SLogicBindRecord> Records;
    xr_vector<SPendingRemoveBinder> ToRemove;
    
    CLogicManager();
public:
    static CLogicManager& GetInstance();

    CLogicManager(CLogicManager const&) = delete;
    CLogicManager& operator=(CLogicManager const&) = delete;
    CLogicManager(CLogicManager&&) = delete;
    CLogicManager& operator=(CLogicManager&&) = delete;

    void Serialize(ISaveObject& Object);

    void OnAquireInfo(shared_str Info);
    void OnReleaseInfo(shared_str Info);

    void OnSignal(ALife::_OBJECT_ID id, shared_str Signal);
    void OnNPCInZone(ALife::_OBJECT_ID id, shared_str ZoneName);
    void OnNPCOutZone(ALife::_OBJECT_ID id, shared_str ZoneName);

    void Update();
};
