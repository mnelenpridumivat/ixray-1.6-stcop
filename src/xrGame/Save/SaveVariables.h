#pragma once

#include "../xrCore/_types.h"
#include "../xrCore/_vector4.h"

enum class ESaveVariableType : u8 {
	t_bool,
	t_float,
	t_double,
	t_vec3,
	t_vec4,
	t_u64,
	t_s64,
	t_u32,
	t_s32,
	t_u16,
	t_s16,
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
	t_chunkSize,
	t_array,
	t_invalid = u8(-1),
};

class ISaveable{
public:
	virtual ESaveVariableType GetVariableType() = 0;
	virtual bool IsArray() = 0;
};

class CSaveVariableBase: public ISaveable {
protected:
	virtual void* GetValue() { return nullptr; }

public:
	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_invalid; }
	virtual bool IsArray() { return false; }
};

class CSaveVariableArray :
	public CSaveVariableBase
{
	u64 _size;
	u64 _currentReadPos;
	xr_vector<xr_unique_ptr<ISaveable>> _array;

public:
	CSaveVariableArray(u64 Size) : _size(Size){}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_array; }
	virtual bool IsArray() { return true; }

	u64 GetSize() { return _size; }
	ISaveable* GetCurrentElement() { VERIFY(_currentReadPos < _size); return _array[_currentReadPos].get(); }
	void Next() { ++_currentReadPos; }

	void AddVariable(ISaveable* data) { _array.push_back(xr_unique_ptr<ISaveable>(data)); }
};

class CSaveVariableBool:
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

class CSaveVariableFloat :
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

class CSaveVariableDouble :
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

class CSaveVariableVec3 :
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

class CSaveVariableVec4 :
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

class CSaveVariableU64 :
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

class CSaveVariableS64 :
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

class CSaveVariableU32 :
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

class CSaveVariableS32 :
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

class CSaveVariableU16 :
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

class CSaveVariableS16 :
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

class CSaveVariableU8 :
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

class CSaveVariableS8 :
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

class CSaveVariableString :
	public CSaveVariableBase
{
	friend struct SSaveVariableGetter;
	xr_string _value;

protected:
	virtual void* GetValue() override { return &_value; }

public:
	CSaveVariableString(const xr_string& Value) : _value(Value) {}
	CSaveVariableString(const shared_str& Value) : _value(Value.c_str()) {}
	CSaveVariableString(LPCSTR Value) : _value(Value) {}

	virtual ESaveVariableType GetVariableType() override { return ESaveVariableType::t_string; }
};

class CSaveVariableMatrix :
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

class CSaveVariableClientID :
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