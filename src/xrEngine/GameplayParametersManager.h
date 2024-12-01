#pragma once
#include "../xrCore/_flags.h"
#include "../xrCore/_types.h"

class ENGINE_API GameplayParametersManager {

	Flags64 _gameplayFlags;

	GameplayParametersManager() = default;

public:

	GameplayParametersManager(const GameplayParametersManager& other) = delete;
	GameplayParametersManager(GameplayParametersManager&& other) = delete;
	GameplayParametersManager& operator=(const GameplayParametersManager& other) = delete;
	GameplayParametersManager& operator=(GameplayParametersManager&& other) = delete;

	static GameplayParametersManager& GetInstance();

	enum class GameplayFlags : u64 {
		use_item_animations = 1
	};

	inline Flags64* GetFlagsPtr() { return &_gameplayFlags; }
	inline bool Test(GameplayFlags value) { return _gameplayFlags.test((u64)value); }

};