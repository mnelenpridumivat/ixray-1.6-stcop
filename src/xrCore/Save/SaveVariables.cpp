#include "stdafx.h"
#include "SaveVariables.h"
#include "MemoryBuffer.h"
#include "SaveManager.h"

CSaveVariableArray::~CSaveVariableArray()
{
	for (size_t i = 0; i < _array.size(); ++i) {
		xr_delete(_array[i]);
	}
}

void CSaveVariableArray::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_array);
	Buffer.Write(_size);
	for (const auto& elem : _array) {
		elem->Write(Buffer);
	}
}

void CSaveVariableBool::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_bool);
	CSaveManager::GetInstance().ConditionalWriteBool(_value, Buffer);
}

void CSaveVariableFloat::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_float);
	Buffer.Write(_value);
}

void CSaveVariableDouble::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_double);
	Buffer.Write(_value);
}

void CSaveVariableU64::Write(CMemoryBuffer& Buffer)
{
	if (CSaveManager::GetInstance().TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseIntOptimization)) {
		if (_value <= std::numeric_limits<u8>::max()) {
			Buffer.Write(ESaveVariableType::t_u64_op8);
			Buffer.Write<u8>(_value);
			return;
		}
		if (_value <= std::numeric_limits<u16>::max()) {
			Buffer.Write(ESaveVariableType::t_u64_op16);
			Buffer.Write<u16>(_value);
			return;
		}
		if (_value <= std::numeric_limits<u32>::max()) {
			Buffer.Write(ESaveVariableType::t_u64_op32);
			Buffer.Write<u32>(_value);
			return;
		}
	}
	Buffer.Write(ESaveVariableType::t_u64);
	Buffer.Write(_value);
}

void CSaveVariableS64::Write(CMemoryBuffer& Buffer)
{
	if (CSaveManager::GetInstance().TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseIntOptimization)) {
		if (_value <= std::numeric_limits<s8>::max() && _value >= std::numeric_limits<s8>::min()) {
			Buffer.Write(ESaveVariableType::t_s64_op8);
			Buffer.Write<s8>(_value);
			return;
		}
		if (_value <= std::numeric_limits<s16>::max() && _value >= std::numeric_limits<s16>::min()) {
			Buffer.Write(ESaveVariableType::t_s64_op16);
			Buffer.Write<s16>(_value);
			return;
		}
		if (_value <= std::numeric_limits<s32>::max() && _value >= std::numeric_limits<s32>::min()) {
			Buffer.Write(ESaveVariableType::t_s64_op32);
			Buffer.Write<s32>(_value);
			return;
		}
	}
	Buffer.Write(ESaveVariableType::t_s64);
	Buffer.Write(_value);
}

void CSaveVariableU32::Write(CMemoryBuffer& Buffer)
{
	if (CSaveManager::GetInstance().TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseIntOptimization)) {
		if (_value <= std::numeric_limits<u8>::max()) {
			Buffer.Write(ESaveVariableType::t_u32_op8);
			Buffer.Write<u8>(_value);
			return;
		}
		if (_value <= std::numeric_limits<u16>::max()) {
			Buffer.Write(ESaveVariableType::t_u32_op16);
			Buffer.Write<u16>(_value);
			return;
		}
	}
	Buffer.Write(ESaveVariableType::t_u32);
	Buffer.Write(_value);
}

void CSaveVariableS32::Write(CMemoryBuffer& Buffer)
{
	if (CSaveManager::GetInstance().TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseIntOptimization)) {
		if (_value <= std::numeric_limits<s8>::max() && _value >= std::numeric_limits<s8>::min()) {
			Buffer.Write(ESaveVariableType::t_s32_op8);
			Buffer.Write<s8>(_value);
			return;
		}
		if (_value <= std::numeric_limits<s16>::max() && _value >= std::numeric_limits<s16>::min()) {
			Buffer.Write(ESaveVariableType::t_s32_op16);
			Buffer.Write<s16>(_value);
			return;
		}
	}
	Buffer.Write(ESaveVariableType::t_s32);
	Buffer.Write(_value);
}

void CSaveVariableU16::Write(CMemoryBuffer& Buffer)
{
	if (CSaveManager::GetInstance().TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseIntOptimization)) {
		if (_value <= std::numeric_limits<u8>::max()) {
			Buffer.Write(ESaveVariableType::t_u16_op8);
			Buffer.Write<u8>(_value);
			return;
		}
	}
	Buffer.Write(ESaveVariableType::t_u16);
	Buffer.Write(_value);
}

void CSaveVariableS16::Write(CMemoryBuffer& Buffer)
{
	if (CSaveManager::GetInstance().TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseIntOptimization)) {
		if (_value <= std::numeric_limits<s8>::max() && _value >= std::numeric_limits<s8>::min()) {
			Buffer.Write(ESaveVariableType::t_s16_op8);
			Buffer.Write<s8>(_value);
			return;
		}
	}
	Buffer.Write(ESaveVariableType::t_s16);
	Buffer.Write(_value);
}

void CSaveVariableU8::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_u8);
	Buffer.Write(_value);
}

void CSaveVariableS8::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_s8);
	Buffer.Write(_value);
}

void CSaveVariableString::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_string);
	CSaveManager::GetInstance().ConditionalWriteString(_value, Buffer);
}

CSaveVariableArrayUnspec::~CSaveVariableArrayUnspec()
{
	for (size_t i = 0; i < _array.size(); ++i) {
		xr_delete(_array[i]);
	}
}

void CSaveVariableArrayUnspec::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_arrayUnspec);
	for (const auto& elem : _array) {
		elem->Write(Buffer);
	}
	Buffer.Write(ESaveVariableType::t_arrayUnspecEnd);
}
