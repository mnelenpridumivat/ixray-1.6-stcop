#pragma once
#include "object_interfaces.h"

enum EBinderParamType {
    eBinderParamString = 0,
    eBinderParamBool,
    eBinderParamDouble,
    eBinderParamInvalid
};

class CBinderParam : public IPureSerializeObject<IReader, IWriter> {
    EBinderParamType type = eBinderParamInvalid;
    using types = std::variant<xr_string, bool, double>;
    types value;

    void DestroyValue();

public:
    CBinderParam(){}
    CBinderParam(const CBinderParam& other);
    CBinderParam(LPCSTR TypeString);
    CBinderParam(bool TypeBool);
    CBinderParam(double TypeDouble);

    ~CBinderParam(){
        DestroyValue();
    }

    CBinderParam& operator=(const CBinderParam& other);

    EBinderParamType GetType() const;
    void SetString(LPCSTR value);
    void SetBool(bool value);
    void SetDouble(double value);
    LPCSTR GetString() const;
    bool GetBool() const;
    double GetDouble() const;

    void save(IWriter& output_packet) override;
    void load(IReader& input_packet) override;
    virtual void Serialize(ISaveObject& Object);
};

ISaveObject& operator<<(ISaveObject& Object, CBinderParam& Value);

class CBinderParams : public IPureSerializeObject<IReader, IWriter> {
    xr_vector<CBinderParam> params = {};

public:
    CBinderParams();
    CBinderParams(const CBinderParams& other);
    CBinderParams(CBinderParams&& other) noexcept;

    CBinderParams& operator=(const CBinderParams& other);

    void Add(const CBinderParam& other);
    void Insert(int Index, const CBinderParam& other);
    void Remove(int Index);
    const CBinderParam& Get(int Index);
    int Size();

    void save(IWriter& output_packet) override;
    void load(IReader& input_packet) override;
    virtual void Serialize(ISaveObject& Object);
};

ISaveObject& operator<<(ISaveObject& Object, CBinderParams& Value);

class CBinder : public IPureSerializeObject<IReader, IWriter>
{
    int m_iTimerStartValue = 0;
    int m_iTimerCurValue = 0;
    u32 m_iStartTime = 0;
    shared_str m_sFuncName;
    CBinderParams m_params;
    bool m_expired = false;
    bool m_bIsActive = false;

protected:
    void OnTimerEnd();

public:
    CBinder() { m_sFuncName = ""; }
    CBinder(shared_str name, const CBinderParams& params, int value) : m_iTimerStartValue(value), m_sFuncName(name)
    {
        m_params = params;
        m_bIsActive = false;
    }

    void save(IWriter& output_packet) override;
    void load(IReader& input_packet) override;
    void Serialize(ISaveObject& Object);

    bool getExpired() const { return m_expired; }

    void Update();
};

ISaveObject& operator<<(ISaveObject& Object, CBinder& Value);

class CBinderManager
{
    xr_vector<xr_unique_ptr<CBinder>> Binders;

    CBinderManager(){}

public:
    CBinderManager(const CBinderManager& other) = delete;
    CBinderManager(CBinderManager&& other) = delete;
    CBinderManager& operator=(const CBinderManager& other) = delete;
    CBinderManager& operator=(CBinderManager&& other) = delete;

    static CBinderManager& GetInstance();

    void CreateBinder(shared_str name, const CBinderParams& params, int value);

    void save(IWriter& output_packet);
    void load(IReader& input_packet);
    virtual void Serialize(ISaveObject& Object);

    void Update();
};