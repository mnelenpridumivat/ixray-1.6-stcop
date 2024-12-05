////////////////////////////////////////////////////////////////////////////
//	Module 		: CustomTimer.cpp
//	Created 	: 15.08.2023
//  Modified 	: 15.08.2023
//	Author		: Dance Maniac (M.F.S. Team)
//	Description : Engine custom timer
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomTimer.h"
#include "object_broker.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
//#include "../xrEngine/device.h"

#include "../xrScripts/script_engine.h"
#include <luabind/luabind.hpp>
#include <luabind/functor.hpp>

CCustomTimerBase::CCustomTimerBase()
{
    m_iTimerStartValue = 0;
    m_iTimerCurValue = 0;
    m_bIsActive = false;
}

CCustomTimerBase::~CCustomTimerBase()
{
    StopCustomTimer();
}

void CCustomTimerBase::StartCustomTimer()
{
    m_bIsActive = true;
    m_iStartTime = ai().alife().time_manager().game_time();

}

void CCustomTimer::StartCustomTimer()
{
    CCustomTimerBase::StartCustomTimer();

#ifdef DEBUG
    Msg("Custom Timer: %s : Started", m_sTimerName);
#endif
}

void CCustomTimerBase::StopCustomTimer()
{
    m_bIsActive = false;
}

void CCustomTimer::StopCustomTimer()
{
    CCustomTimerBase::StopCustomTimer();

#ifdef DEBUG
    Msg("Custom Timer: %s : Stopped (timer value: %d)", m_sTimerName, getCurValue());
#endif
}

void CCustomTimerBase::ResetCustomTimer()
{

    StopCustomTimer();

    m_iTimerCurValue = 0;

    StartCustomTimer();
}

void CCustomTimer::ResetCustomTimer()
{
#ifdef DEBUG
    Msg("Custom Timer: %s : Reset (timer value: %d)", m_sTimerName, getCurValue());
#endif

    CCustomTimerBase::ResetCustomTimer();
}

void CCustomTimerBase::save(IWriter& packet)
{
    save_data(m_iTimerStartValue, packet);
    save_data(m_iTimerCurValue, packet);
    save_data(m_iTimerMode, packet);
    save_data(m_iStartTime, packet);
    save_data(m_bIsActive, packet);
}

void CCustomTimer::save(IWriter& packet)
{
    save_data(m_sTimerName, packet);
    CCustomTimerBase::save(packet);
}

void CBinder::save(IWriter& packet)
{
    CCustomTimerBase::save(packet);
    save_data(m_sFuncName, packet);
    save_data(m_expired, packet);
    m_params.save(packet);
}

void CCustomTimerBase::load(IReader& packet)
{
    load_data(m_iTimerStartValue, packet);
    load_data(m_iTimerCurValue, packet);
    load_data(m_iTimerMode, packet);
    load_data(m_iStartTime, packet);
    load_data(m_bIsActive, packet);
}

void CCustomTimerBase::Save(CSaveObjectSave* Object) const
{
    Object->BeginChunk("CCustomTimerBase");
    {
        Object->GetCurrentChunk()->w_s32(m_iTimerStartValue);
        Object->GetCurrentChunk()->w_s32(m_iTimerCurValue);
        Object->GetCurrentChunk()->w_s32(m_iTimerMode);
        Object->GetCurrentChunk()->w_u32(m_iStartTime);
        Object->GetCurrentChunk()->w_bool(m_bIsActive);
    }
    Object->EndChunk();
}

void CCustomTimerBase::Load(CSaveObjectLoad* Object)
{
    Object->FindChunk("CCustomTimerBase");
    {
        Object->GetCurrentChunk()->r_s32(m_iTimerStartValue);
        Object->GetCurrentChunk()->r_s32(m_iTimerCurValue);
        Object->GetCurrentChunk()->r_s32(m_iTimerMode);
        Object->GetCurrentChunk()->r_u32(m_iStartTime);
        Object->GetCurrentChunk()->r_bool(m_bIsActive);
    }
    Object->EndChunk();
}

void CCustomTimer::load(IReader& packet)
{
    load_data(m_sTimerName, packet);
    CCustomTimerBase::load(packet);
}

void CCustomTimer::Save(CSaveObjectSave* Object) const
{
    Object->BeginChunk("CCustomTimer");
    {
        CCustomTimerBase::Save(Object);
        Object->GetCurrentChunk()->w_stringZ(m_sTimerName);
    }
    Object->EndChunk();
}

void CCustomTimer::Load(CSaveObjectLoad* Object)
{
    Object->FindChunk("CCustomTimer");
    {
        CCustomTimerBase::Load(Object);
        Object->GetCurrentChunk()->r_stringZ(m_sTimerName);
    }
    Object->EndChunk();
}

void CBinder::load(IReader& input_packet)
{
    CCustomTimerBase::load(input_packet);
    load_data(m_sFuncName, input_packet);
    load_data(m_expired, input_packet);
    m_params.load(input_packet);
    m_bIsActive = true;
}

void CBinder::Save(CSaveObjectSave* Object) const
{
    Object->BeginChunk("CBinder");
    {
        CCustomTimerBase::Save(Object);
        Object->GetCurrentChunk()->w_stringZ(m_sFuncName);
        Object->GetCurrentChunk()->w_bool(m_expired);
        m_params.Save(Object);
    }
    Object->EndChunk();
}

void CBinder::Load(CSaveObjectLoad* Object)
{
    Object->FindChunk("CBinder");
    {
        CCustomTimerBase::Load(Object);
        Object->GetCurrentChunk()->r_stringZ(m_sFuncName);
        Object->GetCurrentChunk()->r_bool(m_expired);
        m_params.Load(Object);
        m_bIsActive = true;
    }
    Object->EndChunk();
}

void CCustomTimerBase::Update()
{
    if (!m_bIsActive)
        return;

    m_iTimerCurValue += Device.dwTimeDelta;

    u32 Mult = 1;

    switch (m_iTimerMode)
    {
    case 1: { // seconds
        Mult = 1000;
        break;
    }
    case 2: { // minutes
        Mult = 1000 * 60;
        break;
    }
    case 3: { // hours
        Mult = 1000 * 60 * 60;
        break;
    }
    }

    if (m_iTimerCurValue >= m_iTimerStartValue * Mult){
        OnTimerEnd();
	}
}

void CCustomTimer::OnTimerEnd()
{
     OnTimerStop(m_sTimerName);

     m_bIsActive = false;
     m_iTimerCurValue = 0;

     luabind::functor<void> funct;
     if (ai().script_engine().functor("mfs_functions.on_custom_timer_end", funct)) {
         funct(m_sTimerName.c_str());
     }
}

void CBinder::OnTimerEnd()
{
    m_bIsActive = false;

    luabind::functor<void> funct;
    if (ai().script_engine().functor(m_sFuncName.c_str(), funct)) {
        funct(m_params);
        m_expired = true;
        return;
    }
    Msg("! Unable to process binder with name [%s]!", m_sFuncName.c_str());
}

CTimerManager& CTimerManager::GetInstance()
{
    static CTimerManager manager;
    return manager;
}

void CTimerManager::CreateTimer(shared_str name, int value, int mode)
{
    for (auto& timer : Timers)
    {
        if ((*timer).getName() == name)
        {
            Msg("! Custom Timer with name [%s] already exists!");
            return;
        }
#ifdef DEBUG
        else
        {
            Msg("Custom Timer: %s : Created (start value: %d)", (*timer).getName(), (*timer).getValue());
        }
#endif
    }

    Timers.push_back(std::make_shared<CCustomTimer>(name, value, mode));
}

bool CTimerManager::DeleteTimer(shared_str name)
{
    for (auto it = Timers.begin(); it != Timers.end(); ++it)
    {
        if ((*it)->getName() == name)
        {
            (*it)->StopCustomTimer();
            Timers.erase(it);
            return true;

#ifdef DEBUG
            Msg("Custom Timer: %s : Deleted", (*it)->getName());
#endif
        }
    }
    return false;
}

bool CTimerManager::ResetTimer(shared_str name)
{
    for (auto& timer : Timers)
    {
        if ((*timer).getName() == name)
        {
            (*timer).ResetCustomTimer();
            return true;
        }
    }
    return false;
}

bool CTimerManager::StartTimer(shared_str name, int start_time, int mode)
{
    for (auto& timer : Timers)
    {
        if ((*timer).getName() == name)
        {
            (*timer).SetOnTimerStopCallback([this, name = (*timer).getName()](shared_str stopped_name)
            {
                OnTimerStop(stopped_name);
            });

            if ((*timer).getCurValue() > 0)
                (*timer).ResetCustomTimer();

            if (mode)
                (*timer).setMode(mode);

            if (start_time > 0)
                (*timer).setValue(start_time);

            (*timer).StartCustomTimer();

            return true;
        }
    }
    return false;
}

bool CTimerManager::StopTimer(shared_str name)
{
    for (auto& timer : Timers)
    {
        if ((*timer).getName() == name)
        {
            (*timer).StopCustomTimer();
            return true;
        }
    }
    return false;
}

void CTimerManager::save(IWriter& packet)
{
    u32 timer_count = static_cast<u32>(Timers.size());
    save_data(timer_count, packet);

    for (const auto& timer : Timers)
    {
        timer->save(packet);
    }
}

void CTimerManager::load(IReader& packet)
{
    u32 timer_count = 0;
    load_data(timer_count, packet);

    Timers.clear();

    for (u32 i = 0; i < timer_count; ++i)
    {
        auto timer = xr_make_shared<CCustomTimer>();
        timer->load(packet);
        Timers.push_back(timer);
    }
}

void CTimerManager::Save(CSaveObjectSave* Object) const
{
    Object->BeginChunk("CTimerManager");
    {
        Object->GetCurrentChunk()->WriteArray(Timers.size());
        {
            for (const auto& timer : Timers)
            {
                timer->Save(Object);
            }
        }
        Object->GetCurrentChunk()->EndArray();
    }
    Object->EndChunk();
}

void CTimerManager::Load(CSaveObjectLoad* Object)
{
    Object->FindChunk("CBinderManager");
    {
        Timers.clear();
        u64 ArraySize;
        Object->GetCurrentChunk()->ReadArray(ArraySize);
        {
            for (u64 i = 0; i < ArraySize; ++i)
            {
                Timers.push_back(xr_make_unique<CCustomTimer>());
                Timers.back()->Load(Object);
            }
        }
        Object->GetCurrentChunk()->EndArray();
    }
    Object->EndChunk();
}

int CTimerManager::GetTimerValue(shared_str name) const
{
    for (const auto& timer : Timers)
    {
        if ((*timer).getName() == name)
        {
            return (*timer).getCurValue();
        }
    }

    return -1;
}

void CTimerManager::Update()
{
    for (auto& timer : Timers)
    {
        timer->Update();
    }
}

CBinderManager& CBinderManager::GetInstance()
{
    static CBinderManager manager;
    return manager;
}

void CBinderManager::CreateBinder(shared_str name, const CBinderParams& params, int value, int mode)
{
    Binders.push_back(xr_make_unique<CBinder>(name, params, value, mode));
}

void CBinderManager::save(IWriter& packet)
{
    u32 timer_count = static_cast<u32>(Binders.size());
    save_data(timer_count, packet);

    for (const auto& timer : Binders)
    {
        timer->save(packet);
    }
}

void CBinderManager::load(IReader& packet)
{
    u32 timer_count = 0;
    load_data(timer_count, packet);

    Binders.clear();

    for (u32 i = 0; i < timer_count; ++i)
    {
        Binders.push_back(xr_make_unique<CBinder>());
        Binders.back()->load(packet);
    }
}

void CBinderManager::Save(CSaveObjectSave* Object) const
{
    Object->BeginChunk("CBinderManager");
    {
        Object->GetCurrentChunk()->WriteArray(Binders.size());
        {
            for (const auto& timer : Binders)
            {
                timer->Save(Object);
            }
        }
        Object->GetCurrentChunk()->EndArray();
    }
    Object->EndChunk();
}

void CBinderManager::Load(CSaveObjectLoad* Object)
{
    Object->FindChunk("CBinderManager");
    {
        Binders.clear();
        u64 ArraySize;
        Object->GetCurrentChunk()->ReadArray(ArraySize);
        {
            for (u64 i = 0; i < ArraySize; ++i)
            {
                Binders.push_back(xr_make_unique<CBinder>());
                Binders.back()->Load(Object);
            }
        }
        Object->GetCurrentChunk()->EndArray();
    }
    Object->EndChunk();
}

void CBinderManager::Update()
{
    if (g_pauseMngr.Paused()) {
        return;
    }
    int expired_num = 0;
    auto OrigBindersNum = Binders.size();
    for (size_t i = 0; i < OrigBindersNum; ++i)
    {
        Binders[i]->Update();
        if(Binders[i]->getExpired())
        {
            ++expired_num;
        }
    }
    if(!expired_num)
    {
	    return;
    }
    // move all expired binders at the end of vector ...
    int found_expired = 0;
    for(int i = 0; i < Binders.size() - expired_num; ++i)
    {
        while(Binders[i]->getExpired())
        {
            std::swap(Binders[i], Binders[Binders.size() - 1 - found_expired]);
            ++found_expired;
        }
        if(expired_num == found_expired)
        {
            break;
        }
    }
    // ... and remove them
    Binders.resize(Binders.size() - expired_num);
}

xr_string GetEnumStrName(EBinderParamType type) {
    static bool Inited = false;
    static xr_vector<xr_string> values = {};
    if (!Inited) {
        values.resize(eBinderParamInvalid+1);
        values[eBinderParamString] = "string";
        values[eBinderParamU64] = "u64";
        values[eBinderParamS64] = "s64";
        values[eBinderParamDouble] = "double";
        values[eBinderParamInvalid] = "invalid";
        Inited = true;
    }
    return values[type];
}

void CBinderParam::DestroyValue() {

}

CBinderParam::CBinderParam(const CBinderParam& other)
{
    type = other.type;
    switch (type) {
    case eBinderParamString: {
        value = std::get<xr_string>(other.value);
        break;
    }
    case eBinderParamU64: {
        value = std::get<u64>(other.value);
        break;
    }
    case eBinderParamS64: {
        value = std::get<s64>(other.value);
        break;
    }
    case eBinderParamDouble: {
        value = std::get<double>(other.value);
        break;
    }
    }
}

CBinderParam::CBinderParam(LPCSTR TypeString): type(eBinderParamString)
{
    value = xr_string(TypeString);
}

CBinderParam::CBinderParam(u64 TypeU64) : type(eBinderParamU64)
{
    value = TypeU64;
}

CBinderParam::CBinderParam(s64 TypeS64) : type(eBinderParamS64)
{
    value = TypeS64;
}

CBinderParam::CBinderParam(double TypeDouble) : type(eBinderParamDouble)
{
    value = TypeDouble;
}

CBinderParam& CBinderParam::operator=(const CBinderParam& other)
{
    type = other.type;
    switch (type) {
    case eBinderParamString: {
        value = std::get<xr_string>(other.value);
        break;
    }
    case eBinderParamU64: {
        value = std::get<u64>(other.value);
        break;
    }
    case eBinderParamS64: {
        value = std::get<s64>(other.value);
        break;
    }
    case eBinderParamDouble: {
        value = std::get<double>(other.value);
        break;
    }
    }
    return *this;
}

EBinderParamType CBinderParam::GetType() const
{
    return type;
}

void CBinderParam::SetString(LPCSTR value)
{
    type = eBinderParamString;
    this->value = xr_string(value);
}

void CBinderParam::SetU64(u64 value)
{
    type = eBinderParamU64; 
    this->value = value;
}

void CBinderParam::SetS64(s64 value)
{
    type = eBinderParamS64; 
    this->value = value;
}

void CBinderParam::SetDouble(double value)
{
    type = eBinderParamDouble; 
    this->value = value;
}

LPCSTR CBinderParam::GetString() const
{
    THROW3(type == eBinderParamString, "Attempt to access data as string while it is not a string!", GetEnumStrName(type).c_str());
    return std::get<xr_string>(value).c_str();
}

u64 CBinderParam::GetU64() const
{
    THROW3(type == eBinderParamU64, "Attempt to access data as u64 while it is not a u64!", GetEnumStrName(type).c_str());
    return std::get<u64>(value);
}

s64 CBinderParam::GetS64() const
{
    THROW3(type == eBinderParamS64, "Attempt to access data as s64 while it is not a s64!", GetEnumStrName(type).c_str());
    return std::get<s64>(value);
}

double CBinderParam::GetDouble() const
{
    THROW3(type == eBinderParamDouble, "Attempt to access data as double while it is not a double!", GetEnumStrName(type).c_str());
    return std::get<double>(value);
}

void CBinderParam::save(IWriter& output_packet) const
{
    output_packet.w_u8(type);
    switch (type) {
    case eBinderParamString: {
        output_packet.w_stringZ(std::get<xr_string>(value).c_str());
        break;
    }
    case eBinderParamU64: {
        output_packet.w_u64(std::get<u64>(value));
        break;
    }
    case eBinderParamS64: {
        output_packet.w_s64(std::get<s64>(value));
        break;
    }
    /*case eBinderParamDouble: {
        output_packet.w_double(std::get<double>(value));
        break;
    }*/
    }
}

void CBinderParam::load(IReader& input_packet)
{
    u8 type;
    type = input_packet.r_u8();
    this->type = static_cast<EBinderParamType>(type);
    switch (type) {
    case eBinderParamString: {
        xr_string new_value;
        input_packet.r_stringZ(new_value);
        value = new_value;
        break;
    }
    case eBinderParamU64: {
        value = input_packet.r_u64();
        break;
    }
    case eBinderParamS64: {
        value = input_packet.r_s64();
        break;
    }
    /*case eBinderParamDouble: {
        value = input_packet.r_double();
        break;
    }*/
    }
}

void CBinderParam::Save(CSaveObjectSave* Object) const
{
    Object->BeginChunk("CBinderParam");
    {
        Object->GetCurrentChunk()->w_u8(type);
        switch (type) {
        case eBinderParamString: {
            Object->GetCurrentChunk()->w_stringZ(std::get<xr_string>(value));
            break;
        }
        case eBinderParamU64: {
            Object->GetCurrentChunk()->w_u64(std::get<u64>(value));
            break;
        }
        case eBinderParamS64: {
            Object->GetCurrentChunk()->w_s64(std::get<s64>(value));
            break;
        }
        }
    }
    Object->EndChunk();
}

void CBinderParam::Load(CSaveObjectLoad* Object)
{
    Object->FindChunk("CBinderParam");
    {
        u8 type;
        Object->GetCurrentChunk()->r_u8(type);
        this->type = static_cast<EBinderParamType>(type);
        switch (type) {
        case eBinderParamString: {
            xr_string new_value;
            Object->GetCurrentChunk()->r_stringZ(new_value);
            value = new_value;
            break;
        }
        case eBinderParamU64: {
            u64 new_value;
            Object->GetCurrentChunk()->r_u64(new_value);
            value = new_value;
            break;
        }
        case eBinderParamS64: {
            s64 new_value;
            Object->GetCurrentChunk()->r_s64(new_value);
            value = new_value;
            break;
        }
        }
    }
    Object->EndChunk();
}

CBinderParams::CBinderParams()
{
}

CBinderParams::CBinderParams(const CBinderParams& other)
{
    for (const auto& elem : other.params) {
        params.push_back(elem);
    }
}

CBinderParams::CBinderParams(CBinderParams&& other)
{
    for (const auto& elem : other.params) {
        params.push_back(elem);
    }
}

CBinderParams& CBinderParams::operator=(const CBinderParams& other)
{
    for (const auto& elem : other.params) {
        params.push_back(elem);
    }
    return *this;
}

void CBinderParams::Add(const CBinderParam& other)
{
    params.push_back(other);
}

void CBinderParams::Insert(int Index, const CBinderParam& other)
{
    THROW2(params.size() > Index && Index >= 0, "Attempt to insert value at invalid index!");
    params[Index] = other;
}

void CBinderParams::Remove(int Index)
{
    THROW2(params.size() > Index && Index >= 0, "Attempt to remove value at invalid index!");
    params.erase(params.begin() + Index, params.begin() + Index);
}

const CBinderParam& CBinderParams::Get(int Index)
{
    THROW2(params.size() > Index && Index >= 0, "Attempt to get value at invalid index!");
    return params[Index];
}

int CBinderParams::Size()
{
    return params.size();
}

void CBinderParams::save(IWriter& output_packet) const
{
    output_packet.w_u8(params.size());
    for (const auto& elem : params) {
        elem.save(output_packet);
    }
}

void CBinderParams::load(IReader& input_packet)
{
    auto size = input_packet.r_u8();
    params.resize(size);
    for (auto i = 0; i < size; ++i) {
        params[i].load(input_packet);
    }
}

void CBinderParams::Save(CSaveObjectSave* Object) const
{
    Object->BeginChunk("CBinderParams");
    {
        Object->GetCurrentChunk()->WriteArray(params.size());
        {
            for (const auto& elem : params) {
                elem.Save(Object);
            }
        }
        Object->GetCurrentChunk()->EndArray();
    }
    Object->EndChunk();
}

void CBinderParams::Load(CSaveObjectLoad* Object)
{
    Object->FindChunk("CBinderParams");
    {
        u64 ArraySize;
        Object->GetCurrentChunk()->ReadArray(ArraySize);
        params.resize(ArraySize);
        {
            for (u64 i = 0; i < ArraySize; ++i) {
                params[i].Load(Object);
            }
        }
        Object->GetCurrentChunk()->EndArray();
    }
    Object->EndChunk();
}
