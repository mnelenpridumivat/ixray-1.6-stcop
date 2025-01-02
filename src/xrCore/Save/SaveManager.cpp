#include "stdafx.h"
#include "SaveManager.h"
#include "MemoryBuffer.h"
//#include "../../xrGame/Level.h"
//#include "../../xrGame/Actor.h"
//#include "../../xrGame/ai_space.h"

CSaveManager::CSaveManager() 
{
	SetFlag(ESaveManagerFlagsGeneral::EUseStringOptimization, true);
	SetFlag(ESaveManagerFlagsGeneral::EUseIntOptimization, true);
	SetFlag(ESaveManagerFlagsGeneral::EUseBoolOptimization, true);
	SetFlag(ESaveManagerFlagsGeneral::EHasExtraControlFlags, false);
}

void CSaveManager::SetFlag(ESaveManagerFlagsGeneral Flag, bool Value)
{
	ControlFlagsDefault.set((u8)Flag, Value);
}

bool CSaveManager::TestFlag(ESaveManagerFlagsGeneral Flag)
{
	return ControlFlagsDefault.test((u8)Flag);
}

CSaveManager& CSaveManager::GetInstance()
{
	static CSaveManager instance;
	return instance;
}

bool CSaveManager::IsSaving()
{
	return false;
}

CSaveObjectSave* CSaveManager::BeginSave()
{
	if (SaveData) {
		xr_delete(SaveData);
	}
	SaveData = new CSaveObjectSave();
	return SaveData;
}

CSaveObjectLoad* CSaveManager::BeginLoad(IReader* stream)
{
	ReadHeader(stream);
	ReadStrings(stream);
	ReadBools(stream);
	auto LoadData = new CSaveObjectLoad();
	LoadData->Parse(stream);
	return LoadData;
}

void CSaveManager::WriteSavedData(const string_path& to_file)
{
	SaveWriter = FS.w_open(to_file);
	Buffers.Init();
	StringsHashesMap = xr_make_unique<xr_map<u32, xr_vector<shared_str>>>();
	BoolQueue = xr_make_unique<xr_queue<bool>>();
	CompileData();
	WriteHeader();
	if (TestFlag(ESaveManagerFlagsGeneral::EUseStringOptimization))
	{
		WriteStrings();
	}
	if (TestFlag(ESaveManagerFlagsGeneral::EUseBoolOptimization))
	{
		WriteBools();
	}
	WriteData();
	StringsHashesMap.reset();
	BoolQueue.reset();
	Buffers.Clear();
	FS.w_close(SaveWriter);
}

void CSaveManager::ConditionalWriteString(shared_str Value, CMemoryBuffer& buffer)
{
	if (TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseStringOptimization)) {
		auto StringKey = crc32(Value.c_str(), Value.size());
		u32 StringVecID = 0;
		auto it = StringsHashesMap->find(StringKey);
		if (it == StringsHashesMap->end()) {
			StringsHashesMap->emplace(StringKey, RStringVec{ Value });
		}
		else {
			bool Contains = false;
			for (u32 i = 0; i < it->second.size(); ++i) {
				if (Value == it->second[i]) {
					Contains = true;
					StringVecID = i;
					break;
				}
			}
			if (!Contains) {
				StringVecID = it->second.size();
				it->second.emplace_back(Value);
			}
		}
		u64 StringRefID = ((u64)StringKey << 32) | StringVecID;
		buffer.Write(StringRefID);
	}
	else {
		buffer.Write(Value);
	}
}

void CSaveManager::ConditionalWriteBool(bool Value, CMemoryBuffer& buffer)
{
	if (TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseBoolOptimization)) 
	{
		BoolQueue->push(Value);
		++BoolsNum;
	}
	else {
		buffer.Write(Value);
	}
}

void CSaveManager::ConditionalReadBool(IReader* stream, bool& Value)
{
	if (TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseBoolOptimization))
	{
		Value = BoolQueue->front();
		BoolQueue->pop();
	}
	else {
		stream->r(&Value, sizeof(bool));
	}
}

void CSaveManager::ConditionalReadString(IReader* stream, shared_str& Value)
{
	if (TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseStringOptimization)) {
		u64 StringRefID = stream->r_u64();
		auto StringKey = StringRefID >> 32;
		u32 StringVecID = StringRefID & 0xFFFFFFFF;
		auto it = StringsHashesMap->find(StringKey);
		VERIFY(it != StringsHashesMap->end());
		VERIFY(StringVecID < it->second.size());
		Value = it->second[StringVecID];
	}
	else {
		Value = ReadStringInternal(stream);
	}
}

bool CSaveManager::GetGameInfoFast(IReader* stream, SGameInfoFast& data)
{
	{
		ESaveVariableType type;
		stream->r(&type, sizeof(ESaveVariableType));
		if (type != ESaveVariableType::t_chunk) {
			return false;
		}
	}
	data.m_actor_health = stream->r_float();
	data.m_game_time = stream->r_u64();
	data.m_level_name = ReadStringInternal(stream);
	return true;
}

void CSaveManager::SkipGameInfo(IReader* stream)
{
	SGameInfoFast data;
	GetGameInfoFast(stream, data);
}

void CSaveManager::WriteGameInfo(const SGameInfoFast& data)
{
	GameInfo = data;
}

void CSaveManager::WriteHeader()
{
	Buffers.BufferHeader->Write(ESaveVariableType::t_chunk);
	Buffers.BufferHeader->Write(GameInfo.m_actor_health);
	Buffers.BufferHeader->Write(GameInfo.m_game_time);
	Buffers.BufferHeader->Write(GameInfo.m_level_name);
	Buffers.BufferHeader->Write(ControlFlagsDefault.flags);
	Buffers.BufferHeader->Write(SaveWriter);
}

void CSaveManager::WriteStrings()
{
	Buffers.BufferStrings->Write(ESaveVariableType::t_chunk);
	Buffers.BufferStrings->Write(StringsHashesMap->size());
	Buffers.BufferStrings->Write(ESaveVariableType::t_array);
	for (const auto& elem : *StringsHashesMap.get()) {
		Buffers.BufferStrings->Write(elem.first);
		Buffers.BufferStrings->Write(ESaveVariableType::t_array);
		Buffers.BufferStrings->Write(elem.second.size());
		for (const auto& elem2 : elem.second) {
			Buffers.BufferStrings->Write(elem2);
		}
	}
	Buffers.BufferStrings->Write(SaveWriter);
}

void CSaveManager::WriteBools()
{
	Buffers.BufferBools->Write(ESaveVariableType::t_chunk);
	Buffers.BufferBools->Write(BoolsNum);
	Flags8 Flags;
	u8 WrittenFlags = 0;
	while (!BoolQueue->empty()) {
		auto Ptr = BoolQueue->front();
		Flags.set(1 << WrittenFlags, Ptr);
		++WrittenFlags;
		if (WrittenFlags == 8) {
			Buffers.BufferBools->Write(Flags.get());
			WrittenFlags = 0;
			Flags.zero();
		}
		BoolQueue->pop();
	}
	if (WrittenFlags != 8) {
		Buffers.BufferBools->Write(Flags.get());
	}
	Buffers.BufferBools->Write(SaveWriter);
}

void CSaveManager::WriteData()
{
	Buffers.BufferGeneral->Write(SaveWriter);
}

void CSaveManager::ReadHeader(IReader* stream)
{
	SkipGameInfo(stream);
	ControlFlagsDefault.flags = stream->r_u8();
}

void CSaveManager::ReadStrings(IReader* stream)
{
	{
		ESaveVariableType type;
		stream->r(&type, sizeof(ESaveVariableType));
		VERIFY(type == ESaveVariableType::t_chunk);
	}
	StringsHashesMap = xr_make_unique<xr_map<u32, xr_vector<shared_str>>>();
	auto MapSize = stream->r_u64();
	{
		ESaveVariableType type;
		stream->r(&type, sizeof(ESaveVariableType));
		VERIFY(type == ESaveVariableType::t_array);
	}
	for (u64 i = 0; i < MapSize; ++i) {
		u32 MapKey = stream->r_u32();
		{
			ESaveVariableType type;
			stream->r(&type, sizeof(ESaveVariableType));
			VERIFY(type == ESaveVariableType::t_array);
		}
		auto ArraySize = stream->r_u64();
		for (u64 j = 0; j < ArraySize; ++j) {
			StringsHashesMap->try_emplace(MapKey, xr_vector<shared_str>());
			StringsHashesMap->at(MapKey).emplace_back(ReadStringInternal(stream));
		}
	}
}

void CSaveManager::ReadBools(IReader* stream)
{
	{
		ESaveVariableType type;
		stream->r(&type, sizeof(ESaveVariableType));
		VERIFY(type == ESaveVariableType::t_chunk);
	}
	BoolQueue = xr_make_unique<xr_queue<bool>>();
	BoolsNum = stream->r_u64();
	Flags8 Flags;
	u8 ReadFlags = 8;
	for (u64 i = 0; i < BoolsNum; ++i) {
		if (ReadFlags == 8) {
			Flags.flags = stream->r_u8();
			ReadFlags = 0;
		}
		BoolQueue->push(Flags.bitTest(ReadFlags++));
	}
}

/*void CSaveManager::ReadData(IReader* stream)
{

}*/

void CSaveManager::CompileData()
{
	SaveData->Write(Buffers.BufferGeneral);
}

shared_str CSaveManager::ReadStringInternal(IReader* stream)
{
	/*auto str_size = stream->r_u32();
	VERIFY(str_size < 256);
	string256 Str;
	stream->r(Str, str_size);*/
	shared_str buffer;
	stream->r_stringZ(buffer);
	return buffer;
}

inline void CSaveManager::SMemoryBuffers::Init() {
	VERIFY(!(BufferHeader || BufferStrings || BufferBools || BufferGeneral));
	BufferHeader = new CMemoryBuffer();
	BufferStrings = new CMemoryBuffer();
	BufferBools = new CMemoryBuffer();
	BufferGeneral = new CMemoryBuffer();
}

inline void CSaveManager::SMemoryBuffers::Clear() {
	xr_delete(BufferHeader);
	xr_delete(BufferStrings);
	xr_delete(BufferBools);
	xr_delete(BufferGeneral);
}
