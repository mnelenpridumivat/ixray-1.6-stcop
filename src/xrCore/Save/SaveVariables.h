#pragma once

#include "../xrCore/_types.h"
#include "../xrCore/_vector4.h"

enum class XRCORE_API ESaveVariableType : u8 {
	t_bool,
	t_float,
	t_double,
	t_vec3,
	t_vec4,
	t_u64,
	t_u64_op32,
	t_u64_op16,
	t_u64_op8,
	t_s64,
	t_s64_op32,
	t_s64_op16,
	t_s64_op8,
	t_u32,
	t_u32_op16,
	t_u32_op8,
	t_s32,
	t_s32_op16,
	t_s32_op8,
	t_u16,
	t_u16_op8,
	t_s16,
	t_s16_op8,
	t_u8,
	t_s8,
	/*t_float_q16,
	t_float_q8,
	t_angle16,
	t_angle8,
	t_dir,
	t_sdir,*/
	t_string,
	t_matrix,
	t_clientID,
	t_chunkStart,
	t_chunkEnd,
	t_array,
	t_chunk,
	t_invalid = u8(-1),
};

class XRCORE_API ISaveable{
public:
	virtual ESaveVariableType GetVariableType() = 0;
	virtual bool IsArray() = 0;
	virtual void Write(CMemoryBuffer& Buffer) = 0;
};

class XRCORE_API CSaveVariableBase: public ISaveable {
protected:
	virtual void* GetValue() { return nullptr; }

public:
	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_invalid; }
	virtual bool IsArray() { return false; }
};

class XRCORE_API CSaveVariableArray :
	public CSaveVariableBase
{
	u64 _size;
	u64 _currentReadPos = 0;
	xr_vector<ISaveable*> _array;

public:
	CSaveVariableArray(u64 Size) : _size(Size) {}
	~CSaveVariableArray();

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_array; }
	virtual bool IsArray() { return true; }

	u64 GetSize() { return _size; }
	ISaveable* GetCurrentElement() { VERIFY(_currentReadPos < _size); return _array[_currentReadPos]; }
	void Next() { ++_currentReadPos; }

	void AddVariable(ISaveable* data) { _array.emplace_back(data); }
};

class XRCORE_API CSaveVariableBool:
	public CSaveVariableBase 
{
	friend struct SSaveVariableGetter;
	bool _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableBool(bool Value): _value(Value){}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_bool; }
};

class XRCORE_API CSaveVariableFloat :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	float _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableFloat(float Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_float; }
};

class XRCORE_API CSaveVariableDouble :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	double _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableDouble(double Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_double; }
};

class XRCORE_API CSaveVariableVec3 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	Fvector _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableVec3(const Fvector& Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_vec3; }
};

class XRCORE_API CSaveVariableVec4 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	Fvector4 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableVec4(const Fvector4& Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_vec4; }
};

class XRCORE_API CSaveVariableU64 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	u64 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableU64(u64 Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_u64; }
};

class XRCORE_API CSaveVariableS64 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	s64 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableS64(s64 Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_s64; }
};

class XRCORE_API CSaveVariableU32 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	u32 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableU32(u32 Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_u32; }
};

class XRCORE_API CSaveVariableS32 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	s32 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableS32(s32 Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_s32; }
};

class XRCORE_API CSaveVariableU16 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	u16 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableU16(u16 Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_u16; }
};

class XRCORE_API CSaveVariableS16 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	s16 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableS16(s16 Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_s16; }
};

class XRCORE_API CSaveVariableU8 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	u8 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableU8(u8 Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_u8; }
};

class XRCORE_API CSaveVariableS8 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	s8 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableS8(s8 Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_s8; }
};

/*class CSaveVariableFloatQ16 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	u16 _value;

protected:
	virtual void* GetValue() override { 
		static float UnwrappedValue; // Hack: safe return pointer of local variable 
		A = (float(_value) * (max - min)) / 65535.f + min;		// floating-point-error possible
		VERIFY((A >= min - EPS_S) && (A <= max + EPS_S));
		return &UnwrappedValue;
	}

public:
	CSaveVariableFloatQ16(float Value, float Min, float Max) : _value(Value) {
		VERIFY(Value >= Min && Value <= Max);
		float q = (Value - Min) / (Max - Min);
		_value = u16(iFloor(q * 65535.f + 0.5f));
	}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_float_q16; }
};

class CSaveVariableFloatQ8 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	u8 _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableFloatQ8(float Value, float Min, float Max) : _value(Value) {
		VERIFY(Value >= Min && Value <= Max);
		float q = (Value - Min) / (Max - Min);
		_value = u8(iFloor(q * 255.f + 0.5f));
	}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_float_q8; }
};

class CSaveVariableAngle16 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	float _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableAngle16(float Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_angle16; }
};

class CSaveVariableAngle8 :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	float _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableAngle8(float Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_angle8; }
};

class CSaveVariableDir :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	Fvector _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableDir(const Fvector& Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_dir; }
};

class CSaveVariableSdir :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	Fvector _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableSdir(const Fvector& Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_sdir; }
};*/

class XRCORE_API CSaveVariableString :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	shared_str _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableString(const xr_string& Value) : _value(Value.c_str()) {}
	CSaveVariableString(const shared_str& Value) : _value(Value.c_str()) {}
	CSaveVariableString(LPCSTR Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_string; }
};

class XRCORE_API CSaveVariableMatrix :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	Fmatrix _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableMatrix(const Fmatrix& Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_matrix; }
};

class XRCORE_API CSaveVariableClientID :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	ClientID _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableClientID(ClientID Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_clientID; }
};

struct SSaveVariableGetter {
	template<typename TType, typename TVarClass>
	static TType GetValue(CSaveVariableBase* Var) { return *((TType*)((TVarClass*)Var)->GetValue()); }
};