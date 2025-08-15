#include "stdafx.h"
#include "SaveVariables.h"
#include "MemoryBuffer.h"
#include "SaveManager.h"

ISaveable* ISaveVariableArray::MakeCopy()
{
	ISaveVariableArray* copy = new ISaveVariableArray();
	for (auto elem : _array)
	{
		copy->AddVariable(elem->MakeCopy());
	}
	return copy;
}

void CSaveVariableBool::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
{
	Buffer.Write(ESaveVariableType::t_bool);
	Task->ConditionalWriteBool(_value, Buffer);
}

ISaveable* CSaveVariableBool::MakeCopy()
{
	return new CSaveVariableBool(_value);
}

void CSaveVariableFloat::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
{
	Buffer.Write(ESaveVariableType::t_float);
	Buffer.Write(_value);
}

ISaveable* CSaveVariableFloat::MakeCopy()
{
	return new CSaveVariableFloat(_value);
}

void CSaveVariableDouble::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
{
	Buffer.Write(ESaveVariableType::t_double);
	Buffer.Write(_value);
}

ISaveable* CSaveVariableDouble::MakeCopy()
{
	return new CSaveVariableDouble(_value);
}

void CSaveVariableU64::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
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

ISaveable* CSaveVariableU64::MakeCopy()
{
	return new CSaveVariableU64(_value);
}

void CSaveVariableS64::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
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

ISaveable* CSaveVariableS64::MakeCopy()
{
	return new CSaveVariableS64(_value);
}

void CSaveVariableU32::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
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

ISaveable* CSaveVariableU32::MakeCopy()
{
	return new CSaveVariableU32(_value);
}

void CSaveVariableS32::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
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

ISaveable* CSaveVariableS32::MakeCopy()
{
	return new CSaveVariableS32(_value);
}

void CSaveVariableU16::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
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

ISaveable* CSaveVariableU16::MakeCopy()
{
	return new CSaveVariableU16(_value);
}

void CSaveVariableS16::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
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

ISaveable* CSaveVariableS16::MakeCopy()
{
	return new CSaveVariableS16(_value);
}

void CSaveVariableU8::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
{
	Buffer.Write(ESaveVariableType::t_u8);
	Buffer.Write(_value);
}

ISaveable* CSaveVariableU8::MakeCopy()
{
	return new CSaveVariableU8(_value);
}

void CSaveVariableS8::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
{
	Buffer.Write(ESaveVariableType::t_s8);
	Buffer.Write(_value);
}

ISaveable* CSaveVariableS8::MakeCopy()
{
	return new CSaveVariableS8(_value);
}

void CSaveVariableString::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
{
	Buffer.Write(ESaveVariableType::t_string);
	Task->ConditionalWriteString(_value, Buffer);
}

ISaveable* CSaveVariableString::MakeCopy()
{
	return new CSaveVariableString(_value);
}

ISaveVariableArray::~ISaveVariableArray()
{
	for (size_t i = 0; i < _array.size(); ++i) {
		xr_delete(_array[i]);
	}
}

void ISaveVariableArray::Write(CMemoryBuffer& Buffer, SSaveTask* Task)
{
	Buffer.Write(ESaveVariableType::t_arrayUnspec);
	for (const auto& elem : _array) {
		elem->Write(Buffer, Task);
	}
	Buffer.Write(ESaveVariableType::t_arrayUnspecEnd);
}
