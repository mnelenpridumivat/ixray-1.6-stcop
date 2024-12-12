#include "stdafx.h"
#include "SaveManager.h"
#include "MemoryBuffer.h"

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

void CSaveManager::ConditionalWriteBool(bool& Value, CMemoryBuffer& buffer)
{
	if (TestFlag(CSaveManager::ESaveManagerFlagsGeneral::EUseBoolOptimization)) 
	{
		BoolQueue->push(&Value);
		++BoolsNum;
	}
	else {
		buffer.Write(Value);
	}
}

void CSaveManager::WriteHeader()
{
	Buffers.BufferHeader->Write(ESaveVariableType::t_chunk);
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

void CSaveManager::CompileData()
{
	SaveData->Write(Buffers.BufferGeneral);
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
