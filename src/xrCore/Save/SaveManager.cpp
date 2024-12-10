#include "stdafx.h"
#include "SaveManager.h"
#include "MemoryBuffer.h"

CSaveManager::CSaveManager() 
{
	SetFlag(ESaveManagerFlagsGeneral::EUseStringOptimization, false);
	SetFlag(ESaveManagerFlagsGeneral::EUseIntOptimization, false);
	SetFlag(ESaveManagerFlagsGeneral::EUseBoolOptimization, false);
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
	WriteHeader();
	if (TestFlag(ESaveManagerFlagsGeneral::EUseStringOptimization))
	{
		WriteStrings();
	}
	if (TestFlag(ESaveManagerFlagsGeneral::EUseBoolOptimization))
	{
		WriteBools();
	}
}

void CSaveManager::WriteHeader()
{
	SaveWriter->w_u8(ControlFlagsDefault.flags);
}

void CSaveManager::WriteStrings()
{
}

void CSaveManager::WriteBools()
{
}

void CSaveManager::WriteData()
{
	CMemoryBuffer buffer;
	SaveData->Write(buffer);
}
