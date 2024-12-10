#pragma once
#include "SaveObject.h"

class XRCORE_API SaveManager
{
	SaveManager();

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

	SaveManager(const SaveManager& other) = delete;
	SaveManager(SaveManager&& other) = delete;
	SaveManager& operator=(const SaveManager& other) = delete;
	SaveManager& operator=(SaveManager&& other) = delete;

	static SaveManager& GetInstance();

	bool IsSaving();
	CSaveObjectSave* BeginSave();
	void WriteSavedData(const string_path& to_file);

};