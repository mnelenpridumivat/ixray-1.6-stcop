#include "stdafx.h"
#include "SaveVariables.h"
#include "SaveChunk.h"

#include <magic_enum/magic_enum.hpp>

#include "MemoryBuffer.h"
#include "SaveManager.h"

void ISaveVariableArray::SaveJSON(nlohmann::json& file) const
{
	for (const auto& elem : _array)
	{
		file.push_back(nlohmann::json());
		auto& json = file.back();
		json["type"] = magic_enum::enum_name(elem->GetVariableType());
		elem->SaveJSON(json["data"]);
	}
}

void ISaveVariableArray::LoadJSON(const nlohmann::json& file)
{
	for (const auto& elem : file)
	{
		auto type = magic_enum::enum_cast<ESaveVariableType>(elem["type"].get<std::string>());
		VERIFY(type.has_value());
		xr_string subchunk_name;
		LPCSTR ptr = nullptr;
		if(type.value() == ESaveVariableType::t_chunk)
		{
			subchunk_name = elem["name"].get<xr_string>();
			ptr = subchunk_name.c_str();
		}
		_array.emplace_back(CreateSaveable(type.value(), ptr));
		_array.back()->LoadJSON(elem["data"]);
	}
}

void CSaveVariableBool::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_bool);
	CSaveManager::GetInstance().ConditionalWriteBool(_value, Buffer);
}

void CSaveVariableBool::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableBool::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
}

void CSaveVariableFloat::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_float);
	Buffer.Write(_value);
}

void CSaveVariableFloat::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableFloat::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
}

void CSaveVariableDouble::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_double);
	Buffer.Write(_value);
}

void CSaveVariableDouble::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableDouble::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
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

void CSaveVariableU64::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableU64::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
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

void CSaveVariableS64::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableS64::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
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

void CSaveVariableU32::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableU32::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
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

void CSaveVariableS32::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableS32::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
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

void CSaveVariableU16::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableU16::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
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

void CSaveVariableS16::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableS16::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
}

void CSaveVariableU8::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_u8);
	Buffer.Write(_value);
}

void CSaveVariableU8::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableU8::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
}

void CSaveVariableS8::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_s8);
	Buffer.Write(_value);
}

void CSaveVariableS8::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableS8::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
}

void CSaveVariableString::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_string);
	CSaveManager::GetInstance().ConditionalWriteString(_value, Buffer);
}

void CSaveVariableString::SaveJSON(nlohmann::json& file) const
{
	CSaveVariableBase::SaveJSON(file);
	file["data"] = _value;
}

void CSaveVariableString::LoadJSON(const nlohmann::json& file)
{
	_value = file["data"];
}

ISaveable* CreateSaveable(ESaveVariableType type, LPCSTR chunk_name)
{
	ISaveable* data = nullptr;
	switch (type)
	{
	case ESaveVariableType::t_bool:
		{
			data = new CSaveVariableBool();
			break;
		}
	case ESaveVariableType::t_float:
		{
			data = new CSaveVariableFloat();
			break;
		}
	case ESaveVariableType::t_double:
		{
			data = new CSaveVariableDouble();
			break;
		}
	case ESaveVariableType::t_u64:
		{
			data = new CSaveVariableU64();
			break;
		}
	case ESaveVariableType::t_s64:
		{
			data = new CSaveVariableS64();
			break;
		}
	case ESaveVariableType::t_u32:
		{
			data = new CSaveVariableU32();
			break;
		}
	case ESaveVariableType::t_s32:
		{
			data = new CSaveVariableS32();
			break;
		}
	case ESaveVariableType::t_u16:
		{
			data = new CSaveVariableU16();
			break;
		}
	case ESaveVariableType::t_s16:
		{
			data = new CSaveVariableS16();
			break;
		}
	case ESaveVariableType::t_u8:
		{
			data = new CSaveVariableU8();
			break;
		}
	case ESaveVariableType::t_s8:
		{
			data = new CSaveVariableS8();
			break;
		}
	case ESaveVariableType::t_string:
		{
			data = new CSaveVariableString();
			break;
		}
	case ESaveVariableType::t_arrayUnspec:
		{
			data = new ISaveVariableArray();
			break;
		}
	case ESaveVariableType::t_chunk:
		{
			data = new CSaveChunk(chunk_name);
			break;
		}
	default:
		{
			NODEFAULT;
		}
	}
	VERIFY(data);
	return data;
}

/*void to_json(nlohmann::json& file, ISaveable* data)
{
	data->SaveJSON(file);
}

void from_json(const nlohmann::json& file, ISaveable*& data)
{
	VERIFY(!data);
	auto type = magic_enum::enum_cast<ESaveVariableType>(file["type"].get<std::string>());
	R_ASSERT(type.has_value());
	switch (type.value())
	{
	case ESaveVariableType::t_bool:
		{
			data = new CSaveVariableBool();
			break;
		}
	case ESaveVariableType::t_float:
		{
			data = new CSaveVariableFloat();
			break;
		}
	case ESaveVariableType::t_double:
		{
			data = new CSaveVariableDouble();
			break;
		}
	case ESaveVariableType::t_u64:
		{
			data = new CSaveVariableU64();
			break;
		}
	case ESaveVariableType::t_s64:
		{
			data = new CSaveVariableS64();
			break;
		}
	case ESaveVariableType::t_u32:
		{
			data = new CSaveVariableU32();
			break;
		}
	case ESaveVariableType::t_s32:
		{
			data = new CSaveVariableS32();
			break;
		}
	case ESaveVariableType::t_u16:
		{
			data = new CSaveVariableU16();
			break;
		}
	case ESaveVariableType::t_s16:
		{
			data = new CSaveVariableS16();
			break;
		}
	case ESaveVariableType::t_u8:
		{
			data = new CSaveVariableU8();
			break;
		}
	case ESaveVariableType::t_s8:
		{
			data = new CSaveVariableS8();
			break;
		}
	case ESaveVariableType::t_string:
		{
			data = new CSaveVariableString();
			break;
		}
	case ESaveVariableType::t_arrayUnspec:
		{
			data = new ISaveVariableArray();
			break;
		}
	case ESaveVariableType::t_chunk:
		{
			data = new CSaveChunk(file["name"]);
			break;
		}
	default:
		{
			NODEFAULT;
		}
	}
	data->LoadJSON(file);
}*/

void ISaveable::SaveJSON(nlohmann::json& file) const
{
	file["type"] = magic_enum::enum_name(GetVariableType());
}

ISaveVariableArray::~ISaveVariableArray()
{
	for (size_t i = 0; i < _array.size(); ++i) {
		xr_delete(_array[i]);
	}
}

void ISaveVariableArray::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write(ESaveVariableType::t_arrayUnspec);
	for (const auto& elem : _array) {
		elem->Write(Buffer);
	}
	Buffer.Write(ESaveVariableType::t_arrayUnspecEnd);
}
