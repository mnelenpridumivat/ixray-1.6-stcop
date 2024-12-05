#pragma once

#include <functional>
#include <variant>
#include "Save/SaveObject.h"

class CCustomTimerBase
{
protected:
	int             m_iTimerStartValue;
    int             m_iTimerCurValue;
    int             m_iTimerMode; //0 - milliseconds, 1 - seconds, 2 - minutes, 3 hours
    u32             m_iStartTime;

    CCustomTimerBase(const CCustomTimerBase&) = delete;
    CCustomTimerBase& operator=(const CCustomTimerBase&) = delete;

    bool            m_bIsActive;

    virtual void OnTimerEnd() = 0;

public:
    CCustomTimerBase();
    CCustomTimerBase(int value, int mode = 0) : m_iTimerStartValue(value), m_iTimerMode(mode), m_iTimerCurValue(0), m_iStartTime(0), m_bIsActive(false) {}

    ~CCustomTimerBase();

	virtual void StartCustomTimer   ();
    virtual void StopCustomTimer    ();
    virtual void ResetCustomTimer   ();
    void Update             ();

    void setValue           (int value)         {m_iTimerStartValue = value;}
    int  getValue           () const            {return m_iTimerStartValue;}

    void setCurValue        (int value)         {m_iTimerCurValue = value;}
    int  getCurValue        () const            {return m_iTimerCurValue;}

    void setMode            (int mode)          {m_iTimerMode = mode;}
    int  getMode            () const            {return m_iTimerMode;}

    virtual void save               (IWriter& output_packet);
    virtual void load               (IReader& input_packet);
    virtual void Save(CSaveObjectSave* Object) const;
    virtual void Load(CSaveObjectLoad* Object);
};

class CCustomTimer: public CCustomTimerBase
{
    shared_str     m_sTimerName;
    std::function<void(shared_str)> OnTimerStop = [](shared_str) {};

protected:

    virtual void OnTimerEnd() override;

public:
    CCustomTimer() { m_sTimerName = ""; }
    CCustomTimer(shared_str name, int value, int mode = 0) : m_sTimerName(name), CCustomTimerBase(value, mode) {}


    virtual void StartCustomTimer() override;
    virtual void StopCustomTimer() override;
    virtual void ResetCustomTimer() override;

    virtual void save(IWriter& output_packet) override;
    virtual void load(IReader& input_packet) override;
    virtual void Save(CSaveObjectSave* Object) const override;
    virtual void Load(CSaveObjectLoad* Object) override;

    void setName(shared_str name) { m_sTimerName = name; }
    shared_str getName() const { return m_sTimerName; }

    void SetOnTimerStopCallback(std::function<void(shared_str)> callback)
    {
        OnTimerStop = callback;
    }
};

enum EBinderParamType {
    eBinderParamString = 0,
    eBinderParamU64,
    eBinderParamS64,
    eBinderParamDouble,
    eBinderParamInvalid
};

class CBinderParam {
    EBinderParamType type = eBinderParamInvalid;
    using types = std::variant<xr_string, u64, s64, double>;
    types value;

    void DestroyValue();

public:
    CBinderParam(){}
    CBinderParam(const CBinderParam& other);
    CBinderParam(LPCSTR TypeString);
    CBinderParam(u64 TypeU64);
    CBinderParam(s64 TypeS64);
    CBinderParam(double TypeDouble);

    ~CBinderParam(){
        DestroyValue();
    }

    CBinderParam& operator=(const CBinderParam& other);

    EBinderParamType GetType() const;
    void SetString(LPCSTR value);
    void SetU64(u64 value);
    void SetS64(s64 value);
    void SetDouble(double value);
    LPCSTR GetString() const;
    u64 GetU64() const;
    s64 GetS64() const;
    double GetDouble() const;

    void save(IWriter& output_packet) const;
    void load(IReader& input_packet);
    virtual void Save(CSaveObjectSave* Object) const;
    virtual void Load(CSaveObjectLoad* Object);
};

class CBinderParams {
    xr_vector<CBinderParam> params = {};

public:
    CBinderParams();
    CBinderParams(const CBinderParams& other);
    CBinderParams(CBinderParams&& other);

    CBinderParams& operator=(const CBinderParams& other);

    void Add(const CBinderParam& other);
    void Insert(int Index, const CBinderParam& other);
    void Remove(int Index);
    const CBinderParam& Get(int Index);
    int Size();

    void save(IWriter& output_packet) const;
    void load(IReader& input_packet);
    virtual void Save(CSaveObjectSave* Object) const;
    virtual void Load(CSaveObjectLoad* Object);
};

class CBinder : public CCustomTimerBase
{
    shared_str     m_sFuncName;
    CBinderParams m_params;
    bool m_expired = false;

protected:
    virtual void OnTimerEnd() override;

public:
    CBinder() { m_sFuncName = ""; }
    CBinder(shared_str name, const CBinderParams& params, int value, int mode = 0) : m_sFuncName(name), CCustomTimerBase(value, mode)
    {
        m_params = params;
        StartCustomTimer();
    }

    virtual void save(IWriter& output_packet) override;
    virtual void load(IReader& input_packet) override;
    virtual void Save(CSaveObjectSave* Object) const override;
    virtual void Load(CSaveObjectLoad* Object) override;

    bool getExpired() const { return m_expired; }

};

class CTimerManager
{
    xr_vector<std::shared_ptr<CCustomTimer>> Timers;
    std::function<void(shared_str)> OnTimerStop = [](shared_str) {};

    CTimerManager(){}

public:
    CTimerManager(const CTimerManager& other) = delete;
    CTimerManager(CTimerManager&& other) = delete;
    CTimerManager& operator=(const CTimerManager& other) = delete;
    CTimerManager& operator=(CTimerManager&& other) = delete;

    static CTimerManager& GetInstance();

    void CreateTimer    (shared_str name, int value, int mode = 0);
    bool DeleteTimer    (shared_str name);
    bool ResetTimer     (shared_str name);
    bool StartTimer     (shared_str name, int start_time = 0, int mode = 0);
    bool StopTimer      (shared_str name);

    int  GetTimerValue  (shared_str name) const;

    void save           (IWriter& output_packet);
    void load           (IReader& input_packet);
    virtual void Save(CSaveObjectSave* Object) const;
    virtual void Load(CSaveObjectLoad* Object);

    void Update         ();

    void SetOnTimerStopCallback(std::function<void(shared_str)> callback)
    {
        OnTimerStop = callback;
    }
};

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

    void CreateBinder(shared_str name, const CBinderParams& params, int value, int mode = 0);

    void save(IWriter& output_packet);
    void load(IReader& input_packet);
    virtual void Save(CSaveObjectSave* Object) const;
    virtual void Load(CSaveObjectLoad* Object);

    void Update();
};

