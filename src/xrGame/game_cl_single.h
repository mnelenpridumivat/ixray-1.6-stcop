#pragma once
#include "game_cl_base.h"
#include "../xrScripts/script_export_space.h"

class game_cl_Single :public game_cl_GameState
{
	typedef game_cl_GameState	inherited;
public :
										game_cl_Single			();
	virtual		CUIGameCustom*			createGameUI			();
	virtual		char*					getTeamSection			(int Team);
	virtual		bool					IsServerControlHits		()	{return true;};

	virtual		ALife::_TIME_ID			GetStartGameTime		();
	virtual		ALife::_TIME_ID			GetGameTime				();	
	virtual		float					GetGameTimeFactor		();	
	virtual		void					SetGameTimeFactor		(const float fTimeFactor);

	virtual		ALife::_TIME_ID		GetEnvironmentGameTime		();
	virtual		float				GetEnvironmentGameTimeFactor();
	virtual		void				SetEnvironmentGameTimeFactor(const float fTimeFactor);

	void		OnDifficultyChanged		();
};


// game difficulty
enum ESingleGameDifficulty{
	egdNovice			= 0,
	egdStalker			= 1,
	egdVeteran			= 2,
	egdMaster			= 3,
	egdCount,
	egd_force_u32		= u32(-1)
};

class CSingleGameStats
{
	friend class CCC_GameDifficulty;
	
	ESingleGameDifficulty SingleGameDifficulty = egdStalker;
	xr_token		difficulty_type_token	[5]={
		{ "gd_novice",	egdNovice},
		{ "gd_stalker", egdStalker},
		{ "gd_veteran", egdVeteran},
		{ "gd_master", egdMaster},
		{ 0, 0}
	};
	bool UseMagazines = false;

	CSingleGameStats(){}

public:

	CSingleGameStats(const CSingleGameStats& stats) = delete;
	CSingleGameStats(CSingleGameStats&& stats) = delete;
	CSingleGameStats& operator=(const CSingleGameStats& stats) = delete;
	CSingleGameStats& operator=(CSingleGameStats&& stats) = delete;

	static CSingleGameStats& GetInstance();

	ESingleGameDifficulty GetSingleGameDifficulty(){return SingleGameDifficulty;}
	LPCSTR GetDifficultyTypeString(){return get_token_name(difficulty_type_token, SingleGameDifficulty);}
	void SetSingleGameDifficulty(ESingleGameDifficulty dif);

	bool GetUseMagazines(){return UseMagazines;}
	void SetUseMagazines(bool b){UseMagazines = b;}

	void Serialize(ISaveObject& Object);
};

//extern ESingleGameDifficulty g_SingleGameDifficulty;
//xr_token		difficulty_type_token	[ ];

typedef enum_exporter<ESingleGameDifficulty> CScriptGameDifficulty;