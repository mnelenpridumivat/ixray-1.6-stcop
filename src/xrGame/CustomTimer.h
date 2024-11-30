#pragma once

#include <functional>
#include <variant>

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
};

class CCustomTimer: public CCustomTimerBase
{
    std::string     m_sTimerName;
    std::function<void(std::string)> OnTimerStop = [](std::string) {};

protected:

    virtual void OnTimerEnd() override;

public:
    CCustomTimer() { m_sTimerName = ""; }
    CCustomTimer(std::string name, int value, int mode = 0) : m_sTimerName(name), CCustomTimerBase(value, mode) {}


    virtual void StartCustomTimer() override;
    virtual void StopCustomTimer() override;
    virtual void ResetCustomTimer() override;

    virtual void save(IWriter& output_packet) override;
    virtual void load(IReader& input_packet) override;

    void setName(std::string name) { m_sTimerName = name; }
    std::string getName() const { return m_sTimerName; }

    void SetOnTimerStopCallback(std::function<void(std::string)> callback)
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
};

class CBinder : public CCustomTimerBase
{
    std::string     m_sFuncName;
    CBinderParams m_params;
    bool m_expired = false;

protected:
    virtual void OnTimerEnd() override;

public:
    CBinder() { m_sFuncName = ""; }
    CBinder(std::string name, const CBinderParams& params, int value, int mode = 0) : m_sFuncName(name), CCustomTimerBase(value, mode)
    {
        m_params = params;
        StartCustomTimer();
    }

    virtual void save(IWriter& output_packet) override;
    virtual void load(IReader& input_packet) override;

    bool getExpired() const { return m_expired; }

};

class CTimerManager
{
    std::vector<std::shared_ptr<CCustomTimer>> Timers;
    std::function<void(std::string)> OnTimerStop = [](std::string) {};

    CTimerManager(){}

public:
    CTimerManager(const CTimerManager& other) = delete;
    CTimerManager(CTimerManager&& other) = delete;
    CTimerManager& operator=(const CTimerManager& other) = delete;
    CTimerManager& operator=(CTimerManager&& other) = delete;

    static CTimerManager& GetInstance();

    void CreateTimer    (std::string name, int value, int mode = 0);
    bool DeleteTimer    (std::string name);
    bool ResetTimer     (std::string name);
    bool StartTimer     (std::string name, int start_time = 0, int mode = 0);
    bool StopTimer      (std::string name);

    int  GetTimerValue  (std::string name) const;

    void save           (IWriter& output_packet);
    void load           (IReader& input_packet);

    void Update         ();

    void SetOnTimerStopCallback(std::function<void(std::string)> callback)
    {
        OnTimerStop = callback;
    }
};

class CBinderManager
{
    std::vector<std::unique_ptr<CBinder>> Binders;

    CBinderManager(){}

public:
    CBinderManager(const CBinderManager& other) = delete;
    CBinderManager(CBinderManager&& other) = delete;
    CBinderManager& operator=(const CBinderManager& other) = delete;
    CBinderManager& operator=(CBinderManager&& other) = delete;

    static CBinderManager& GetInstance();

    void CreateBinder(std::string name, const CBinderParams& params, int value, int mode = 0);

    void save(IWriter& output_packet);
    void load(IReader& input_packet);

    void Update();
};

