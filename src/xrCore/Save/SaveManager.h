#pragma once
#include "SaveObject.h"

class XRCORE_API CSaveManager
{
	CSaveManager();

	CSaveObjectSave* SaveData = nullptr;
	IWriter* SaveWriter = nullptr;
	Flags8 ControlFlagsDefault;
	Flags8 ControlFlagsRead;

	void WriteHeader();
	void WriteStrings();
	void WriteBools();
	void WriteData();

public:

	enum class ESaveManagerFlagsGeneral : u8 {
		EUseStringOptimization = 1,
		EUseIntOptimization = 1 << 1,
		EUseBoolOptimization = 1 << 2,
		EHasExtraControlFlags = 1 << 7
	};

	void SetFlag(ESaveManagerFlagsGeneral Flag, bool Value);
	bool TestFlag(ESaveManagerFlagsGeneral Flag);

	CSaveManager(const CSaveManager& other) = delete;
	CSaveManager(CSaveManager&& other) = delete;
	CSaveManager& operator=(const CSaveManager& other) = delete;
	CSaveManager& operator=(CSaveManager&& other) = delete;

	static CSaveManager& GetInstance();

	bool IsSaving();
	CSaveObjectSave* BeginSave();
	void WriteSavedData(const string_path& to_file);

};