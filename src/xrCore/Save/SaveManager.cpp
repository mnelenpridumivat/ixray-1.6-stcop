#include "stdafx.h"
#include "SaveManager.h"

SaveManager::SaveManager() 
{
	SetFlag(ESaveManagerFlagsGeneral::EUseStringOptimization, false);
	SetFlag(ESaveManagerFlagsGeneral::EUseIntOptimization, false);
	SetFlag(ESaveManagerFlagsGeneral::EUseBoolOptimization, false);
	SetFlag(ESaveManagerFlagsGeneral::EHasExtraControlFlags, false);
}

void SaveManager::SetFlag(ESaveManagerFlagsGeneral Flag, bool Value)
{
	ControlFlagsDefault.set((u8)Flag, Value);
}

bool SaveManager::TestFlag(ESaveManagerFlagsGeneral Flag)
{
	return ControlFlagsDefault.test((u8)Flag);
}

SaveManager& SaveManager::GetInstance()
{
	static SaveManager instance;
	return instance;
}

bool SaveManager::IsSaving()
{
	return false;
}

CSaveObjectSave* SaveManager::BeginSave()
{
	if (SaveData) {
		xr_delete(SaveData);
	}
	SaveData = new CSaveObjectSave();
	return SaveData;
}

void SaveManager::WriteSavedData(const string_path& to_file)
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

void SaveManager::WriteHeader()
{
	SaveWriter->w_u8(ControlFlagsDefault.flags);
}

void SaveManager::WriteStrings()
{
}

void SaveManager::WriteBools()
{
}

void SaveManager::WriteData()
{

}
