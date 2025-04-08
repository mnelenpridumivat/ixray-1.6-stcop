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
    struct SBinderObjectRecord;
    
    struct SBinderConditionBase
    {
        struct MessageBase
        {;
            ALife::_OBJECT_ID id = ALife::_OBJECT_ID(-1);
        };
        virtual ~SBinderConditionBase() = default;
        bool Satisfied = false;
        SBinderObjectRecord* Record = nullptr;
        virtual void Execute(MessageBase* data);
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

    struct SBinderObjectRecord
    {
        ALife::_OBJECT_ID id;
        shared_str Callback;
        shared_str NextSection;
        xr_vector<SBinderConditionBase*> Conditions;

        void VerifyConditions();
    };

    struct SEventInfo{
        shared_str Info;
    };
    
    struct SEventNPC
    {
        ALife::_OBJECT_ID id;
        shared_str Argument;
    };

    struct SBindersRow
    {
        xr_hash_map<ALife::_OBJECT_ID, xr_vector<SBinderConditionBase*>> CondsPerObj;
    };

    xr_hash_map<shared_str,SBindersRow> Binders;
    xr_hash_set<SBinderObjectRecord*> Records;

    xrCriticalSection ProcessLock;
    xr_hash_set<SBinderObjectRecord*> ToProcess;
    
    xrCriticalSection RemoveLock;
    xr_hash_set<SBinderObjectRecord*> ToRemove;

    xrCriticalSection AquireInfoLock;
    xr_vector<SEventInfo> AquiredInfos;
    
    xrCriticalSection ReleasedInfoLock;
    xr_vector<SEventInfo> ReleasedInfos;

    xrCriticalSection SignalLock;
    xr_vector<SEventNPC> HappenedSignals;

    xrCriticalSection NPCInZoneLock;
    xr_vector<SEventNPC> NPCsInZone;

    xrCriticalSection NPCOutZoneLock;
    xr_vector<SEventNPC> NPCsOutZone;
    
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

    void Update(); // For main game thread
    void Update2(); // Not for main game thread
};
