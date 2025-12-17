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

void CBinderHandler::save(IWriter& output_packet)
{
    save_data(m_timer_id, output_packet);
}

void CBinderHandler::load(IReader& input_packet)
{
    load_data(m_timer_id, input_packet);
}

void CBinderHandler::Serialize(ISaveObject& Object)
{
    BEGIN_CHUNK(Object,"CBinderHandler")
    {
        Object << m_timer_id;
    }
}

void CBinder::OnTimerEnd()
{
    m_bIsActive = false;

    switch (m_value.index())
    {
        case 0:
            {
                luabind::functor<void> funct;
                if (ai().script_engine().functor(std::get<Func>(m_value).m_sFuncName.c_str(), funct)) {
                    luabind::object obj = luabind::newtable(ai().script_engine().lua());
                    for (int i = 0; i < std::get<Func>(m_value).m_params.Size(); ++i)
                    {
                        auto& elem = std::get<Func>(m_value).m_params.Get(i);
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
                    if (!m_expired)
                    {
                        funct(obj);
                    }
                    m_expired = true;
                    return;
                }
                Msg("! Unable to process binder with name [%s]!", std::get<Func>(m_value).m_sFuncName.c_str());
                break;
            }
        case 1:
            {
                if (!m_expired)
                {
                    std::get<luabind::object>(m_value)();
                }
                m_expired = true;
                break;
            }
        default: NODEFAULT;
    }
}

bool CBinder::IsSaveable()
{
    return m_value.index() == 0;
}

void CBinder::save(IWriter& packet)
{
    VERIFY(IsSaveable());
    save_data(m_id, packet);
    save_data(m_iTimerStartValue, packet);
    save_data(m_iTimerCurValue, packet);
    save_data(m_iStartTime, packet);
    save_data(m_bIsActive, packet);
    save_data(std::get<Func>(m_value).m_sFuncName, packet);
    save_data(m_expired, packet);
    save_data(m_looped, packet);
    std::get<Func>(m_value).m_params.save(packet);
}

void CBinder::load(IReader& input_packet)
{
    m_value = Func();
    load_data(m_id, input_packet);
    load_data(m_iTimerStartValue, input_packet);
    load_data(m_iTimerCurValue, input_packet);
    load_data(m_iStartTime, input_packet);
    load_data(m_bIsActive, input_packet);
    load_data(std::get<Func>(m_value).m_sFuncName, input_packet);
    load_data(m_expired, input_packet);
    load_data(m_looped, input_packet);
    std::get<Func>(m_value).m_params.load(input_packet);
    m_bIsActive = true;
}

void CBinder::Serialize(ISaveObject& Object)
{
    BEGIN_CHUNK(Object,"CBinder")
    {
        Object << m_id << std::get<Func>(m_value).m_sFuncName << m_expired << m_iTimerStartValue << m_iTimerCurValue << m_iStartTime << m_bIsActive;
        std::get<Func>(m_value).m_params.Serialize(Object);
        if (!Object.IsSave()) {
            m_bIsActive = true;
        }
    }
}

void CBinder::Update()
{
    if (!m_bIsActive)
    {
        return;
    }

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

CBinderHandler CBinderManager::CreateBinder(shared_str name, const CBinderParams& params, int value, bool looped)
{
    auto id = m_id_gen++;
    Binders.push_back(xr_make_unique<CBinder>(id, name, params, value, looped));
    return id;
}

CBinderHandler CBinderManager::CreateBinder(luabind::object func, int value, bool looped)
{
    auto id = m_id_gen++;
    Binders.push_back(xr_make_unique<CBinder>(id, func, value, looped));
    return id;
}

bool CBinderManager::IsTimerValid(CBinderHandler handler)
{
    for (auto& elem : Binders)
    {
        if (elem->getId() == handler.GetID())
        {
            return true;
        }
    }
    return false;
}

void CBinderManager::Pause(CBinderHandler handler)
{
    R_ASSERT(IsTimerValid(handler));
    for (auto& elem : Binders)
    {
        if (elem->getId() == handler.GetID())
        {
            elem->Pause();
            return;
        }
    }
}

void CBinderManager::Resume(CBinderHandler handler)
{
    R_ASSERT(IsTimerValid(handler));
    for (auto& elem : Binders)
    {
        if (elem->getId() == handler.GetID())
        {
            elem->Resume();
            return;
        }
    }
}

void CBinderManager::Stop(CBinderHandler handler)
{
    R_ASSERT(IsTimerValid(handler));
    for (auto& elem : Binders)
    {
        if (elem->getId() == handler.GetID())
        {
            elem->Stop();
            return;
        }
    }
}

void CBinderManager::save(IWriter& packet)
{
    save_data(m_id_gen, packet);
    xr_vector<CBinder*> Copy;
    for (auto& elem : Binders)
    {
        Copy.push_back(elem.get());
    }

    std::erase_if(Copy, [](CBinder* binder){return !binder->IsSaveable();});
    
    u32 timer_count = static_cast<u32>(Copy.size());
    save_data(timer_count, packet);

    for (const auto& timer : Copy)
    {
        timer->save(packet);
    }
}

void CBinderManager::load(IReader& packet)
{
    load_data(m_id_gen, packet);
    u32 timer_count = 0;
    load_data(timer_count, packet);

    Binders.clear();

    for (u32 i = 0; i < timer_count; ++i)
    {
        Binders.push_back(xr_make_unique<CBinder>());
        Binders.back()->load(packet);
    }
    LoadFinished = false;
}

void CBinderManager::Serialize(ISaveObject& Object)
{
    //if (!Object.IsSave())
    //{
    //    Binders.clear();
    //}
    BEGIN_CHUNK(Object,"CBinderManager")
    {
        Object << m_id_gen << Binders;
    }
    if (!Object.IsSave())
    {
        LoadFinished = false;
    }
}

void CBinderManager::Update()
{
    if (g_pauseMngr.Paused() || !LoadFinished) {
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
    std::erase_if(Binders, [](const xr_unique_ptr<CBinder>& binder){return binder->getExpired() && !binder->getLooped();});
    for (auto& binder : Binders)
    {
        if (binder->getExpired())
        {
            VERIFY(binder->getLooped());
            binder->ResetTimer();
        }
    }
}
