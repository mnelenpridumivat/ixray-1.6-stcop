#include "StdAfx.h"
#include "CustomTimer.h"

#include <magic_enum/magic_enum.hpp>

#include "ai_space.h"
#include "object_loader.h"
#include "object_saver.h"

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
    case eBinderParamBool: {
        value = std::get<bool>(other.value);
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

CBinderParam::CBinderParam(bool TypeBool) : type(eBinderParamBool)
{
    value = TypeBool;
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
    case eBinderParamBool: {
        value = std::get<bool>(other.value);
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

void CBinderParam::SetBool(bool value)
{
    type = eBinderParamBool; 
    this->value = value;
}

void CBinderParam::SetDouble(double value)
{
    type = eBinderParamDouble; 
    this->value = value;
}

LPCSTR CBinderParam::GetString() const
{
    THROW3(type == eBinderParamString, "Attempt to access data as string while it is not a string!", magic_enum::enum_name(type).data());
    return std::get<xr_string>(value).c_str();
}

bool CBinderParam::GetBool() const
{
    THROW3(type == eBinderParamBool, "Attempt to access data as s64 while it is not a s64!", magic_enum::enum_name(type).data());
    return std::get<bool>(value);
}

double CBinderParam::GetDouble() const
{
    THROW3(type == eBinderParamDouble, "Attempt to access data as double while it is not a double!", magic_enum::enum_name(type).data());
    return std::get<double>(value);
}

void CBinderParam::save(IWriter& output_packet)
{
    output_packet.w_u8(type);
    switch (type) {
    case eBinderParamString: {
        output_packet.w_stringZ(std::get<xr_string>(value).c_str());
        break;
    }
    case eBinderParamBool: {
        output_packet.w_u8(std::get<bool>(value));
        break;
    }
    case eBinderParamDouble: {
        output_packet.w_double(std::get<double>(value));
        break;
    }
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
    case eBinderParamBool: {
        value = !!input_packet.r_u8();
        break;
    }
    case eBinderParamDouble: {
        value = input_packet.r_double();
        break;
    }
    }
}

void CBinderParam::Serialize(ISaveObject& Object)
{
    BEGIN_CHUNK(Object,"CBinderParam")
    {
        u8* ValueType = (u8*)&type;
        Object << *ValueType;
        switch (type) {
        case eBinderParamString: {
            xr_string new_value = std::get<xr_string>(value);
            Object << new_value;
            value = new_value;
            break;
        }
        case eBinderParamBool: {
            bool new_value = std::get<bool>(value);
            Object << new_value;
            value = new_value;
            break;
        }
        case eBinderParamDouble: {
            double new_value = std::get<double>(value);
            Object << new_value;
            value = new_value;
            break;
        }
        }
    }
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

CBinderParams::CBinderParams(CBinderParams&& other) noexcept
{
    params = std::move(other.params);
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
    R_ASSERT(params.size() > Index && Index >= 0, "Attempt to insert value at invalid index!");
    params[Index] = other;
}

void CBinderParams::Remove(int Index)
{
    R_ASSERT(params.size() > Index && Index >= 0, "Attempt to remove value at invalid index!");
    params.erase(params.begin() + Index, params.begin() + Index);
}

const CBinderParam& CBinderParams::Get(int Index)
{
    R_ASSERT(params.size() > Index && Index >= 0, "Attempt to get value at invalid index!");
    return params[Index];
}

int CBinderParams::Size()
{
    return params.size();
}

void CBinderParams::save(IWriter& output_packet)
{
    output_packet.w_u8(params.size());
    for (auto& elem : params) {
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

void CBinderParams::Serialize(ISaveObject& Object)
{
    BEGIN_CHUNK(Object,"CBinderParams")
    {
        Object << params;
    }
}

ISaveObject& operator<<(ISaveObject& Object, CBinder& Value)
{
    Value.Serialize(Object);
    return Object;
}

ISaveObject& operator<<(ISaveObject& Object, CBinderParam& Value)
{
    Value.Serialize(Object);
    return Object;
}

ISaveObject& operator<<(ISaveObject& Object, CBinderParams& Value)
{
    Value.Serialize(Object);
    return Object;
}

void CBinder::OnTimerEnd()
{
    m_bIsActive = false;

    luabind::functor<void> funct;
    if (ai().script_engine().functor(m_sFuncName.c_str(), funct)) {
        luabind::object obj = luabind::newtable(ai().script_engine().lua());
        for (int i = 0; i < m_params.Size(); ++i)
        {
            auto& elem = m_params.Get(i);
            switch (elem.GetType())
            {
            case eBinderParamString:
                {
                    obj[i+1] = elem.GetString();
                    break;
                }
            case eBinderParamBool:
                {
                    obj[i+1] = elem.GetBool();
                    break;
                }
            case eBinderParamDouble:
                {
                    obj[i+1] = elem.GetDouble();
                    break;
                }
            }
        }
        funct(obj);
        m_expired = true;
        return;
    }
    Msg("! Unable to process binder with name [%s]!", m_sFuncName.c_str());
}

void CBinder::save(IWriter& packet)
{
    save_data(m_iTimerStartValue, packet);
    save_data(m_iTimerCurValue, packet);
    save_data(m_iStartTime, packet);
    save_data(m_bIsActive, packet);
    save_data(m_sFuncName, packet);
    save_data(m_expired, packet);
    m_params.save(packet);
}

void CBinder::load(IReader& input_packet)
{
    load_data(m_iTimerStartValue, input_packet);
    load_data(m_iTimerCurValue, input_packet);
    load_data(m_iStartTime, input_packet);
    load_data(m_bIsActive, input_packet);
    load_data(m_sFuncName, input_packet);
    load_data(m_expired, input_packet);
    m_params.load(input_packet);
    m_bIsActive = true;
}

void CBinder::Serialize(ISaveObject& Object)
{
    BEGIN_CHUNK(Object,"CBinder")
    {
        Object << m_sFuncName << m_expired << m_iTimerStartValue << m_iTimerCurValue << m_iStartTime << m_bIsActive;
        m_params.Serialize(Object);
        if (!Object.IsSave()) {
            m_bIsActive = true;
        }
    }
}

void CBinder::Update()
{
    if (!m_bIsActive)
        return;

    m_iTimerCurValue += Device.dwTimeDelta;

    if (m_iTimerCurValue >= m_iTimerStartValue){
        OnTimerEnd();
	}
}

CBinderManager& CBinderManager::GetInstance()
{
    static CBinderManager instance;
    return instance;
}

void CBinderManager::CreateBinder(shared_str name, const CBinderParams& params, int value)
{
    Binders.push_back(xr_make_unique<CBinder>(name, params, value));
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

void CBinderManager::Serialize(ISaveObject& Object)
{
    BEGIN_CHUNK(Object,"CBinderManager")
    {
        Object << Binders;
    }
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
