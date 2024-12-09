#include "StdAfx.h"
#include "alife_abstract_registry.h"

#include "game_news.h"
#include "encyclopedia_article_defs.h"
#include "character_info_defs.h"
#include "relation_registry_defs.h"
#include "InfoPortionDefs.h"
#include "_stl_extensions.h"
#include "map_location_defs.h"
#include "map_location.h"
#include "GameTaskDefs.h"
#include "actor_statistic_defs.h"

namespace SaveSystemDefined {

	template<>
	void Save<u16, KNOWN_INFO_VECTOR>(CSaveObjectSave* Obj, u16 Key, const KNOWN_INFO_VECTOR& Value) {
		Obj->GetCurrentChunk()->w_u16(Key);
		Obj->GetCurrentChunk()->WriteArray(Value.size());
		{
			for (const auto& elem : Value) {
				elem.save(Obj);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Load<u16, KNOWN_INFO_VECTOR>(CSaveObjectLoad* Obj, u16& Key, KNOWN_INFO_VECTOR& Value) {
		Obj->GetCurrentChunk()->r_u16(Key);
		Value.clear();
		u64 ArraySize;
		Obj->GetCurrentChunk()->ReadArray(ArraySize);
		{
			for (u64 i = 0; i < ArraySize; ++i) {
				INFO_DATA&& temp = INFO_DATA();
				temp.load(Obj);
				Value.emplace_back(temp);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Save<u16, RELATION_DATA>(CSaveObjectSave* Obj, u16 Key, const RELATION_DATA& Value) {
		Obj->GetCurrentChunk()->w_u16(Key);
		Value.save(Obj);
	}

	template<>
	void Load<u16, RELATION_DATA>(CSaveObjectLoad* Obj, u16& Key, RELATION_DATA& Value) {
		Obj->GetCurrentChunk()->r_u16(Key);
		Value.load(Obj);
	}

	template<>
	void Save<u16, ARTICLE_VECTOR>(CSaveObjectSave* Obj, u16 Key, const ARTICLE_VECTOR& Value) {
		Obj->GetCurrentChunk()->w_u16(Key);
		Obj->GetCurrentChunk()->WriteArray(Value.size());
		{
			for (const auto& elem : Value) {
				elem.save(Obj);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Load<u16, ARTICLE_VECTOR>(CSaveObjectLoad* Obj, u16& Key, ARTICLE_VECTOR& Value) {
		Obj->GetCurrentChunk()->r_u16(Key);
		Value.clear();
		u64 ArraySize;
		Obj->GetCurrentChunk()->ReadArray(ArraySize);
		{
			for (u64 i = 0; i < ArraySize; ++i) {
				ARTICLE_DATA&& temp = ARTICLE_DATA();
				temp.load(Obj);
				Value.emplace_back(temp);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Save<u16, GAME_NEWS_VECTOR>(CSaveObjectSave* Obj, u16 Key, const GAME_NEWS_VECTOR& Value) {
		Obj->GetCurrentChunk()->w_u16(Key);
		Obj->GetCurrentChunk()->WriteArray(Value.size());
		{
			for (const auto& elem : Value) {
				elem.save(Obj);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Load<u16, GAME_NEWS_VECTOR>(CSaveObjectLoad* Obj, u16& Key, GAME_NEWS_VECTOR& Value) {
		Obj->GetCurrentChunk()->r_u16(Key);
		Value.clear();
		u64 ArraySize;
		Obj->GetCurrentChunk()->ReadArray(ArraySize);
		{
			for (u64 i = 0; i < ArraySize; ++i) {
				GAME_NEWS_DATA&& temp = GAME_NEWS_DATA();
				temp.load(Obj);
				Value.emplace_back(temp);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Save<shared_str, int>(CSaveObjectSave* Obj, shared_str Key, const int& Value) {
		Obj->GetCurrentChunk()->w_stringZ(Key);
		Obj->GetCurrentChunk()->w_s32(Value);
	}

	template<>
	void Load<shared_str, int>(CSaveObjectLoad* Obj, shared_str& Key, int& Value) {
		Obj->GetCurrentChunk()->r_stringZ(Key);
		Obj->GetCurrentChunk()->r_s32(Value);
	}

	template<>
	void Save<u16, Locations>(CSaveObjectSave* Obj, u16 Key, const Locations& Value) {
		Obj->GetCurrentChunk()->w_u16(Key);
		Obj->GetCurrentChunk()->WriteArray(Value.size());
		{
			for (const auto& elem : Value) {
				elem.save(Obj);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Load<u16, Locations>(CSaveObjectLoad* Obj, u16& Key, Locations& Value) {
		Obj->GetCurrentChunk()->r_u16(Key);
		Value.clear();
		u64 ArraySize;
		Obj->GetCurrentChunk()->ReadArray(ArraySize);
		{
			for (u64 i = 0; i < ArraySize; ++i) {
				SLocationKey&& temp = SLocationKey();
				temp.load(Obj);
				Value.emplace_back(temp);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Save<u16, vGameTasks>(CSaveObjectSave* Obj, u16 Key, const vGameTasks& Value) {
		Obj->GetCurrentChunk()->w_u16(Key);
		Obj->GetCurrentChunk()->WriteArray(Value.size());
		{
			for (const auto& elem : Value) {
				elem.save(Obj);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Load<u16, vGameTasks>(CSaveObjectLoad* Obj, u16& Key, vGameTasks& Value) {
		Obj->GetCurrentChunk()->r_u16(Key);
		Value.clear();
		u64 ArraySize;
		Obj->GetCurrentChunk()->ReadArray(ArraySize);
		{
			for (u64 i = 0; i < ArraySize; ++i) {
				SGameTaskKey&& temp = SGameTaskKey();
				temp.load(Obj);
				Value.emplace_back(temp);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Save<u16, vStatSectionData>(CSaveObjectSave* Obj, u16 Key, const vStatSectionData& Value) {
		Obj->GetCurrentChunk()->w_u16(Key);
		Obj->GetCurrentChunk()->WriteArray(Value.size());
		{
			for (const auto& elem : Value) {
				elem.save(Obj);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

	template<>
	void Load<u16, vStatSectionData>(CSaveObjectLoad* Obj, u16& Key, vStatSectionData& Value) {
		Obj->GetCurrentChunk()->r_u16(Key);
		Value.clear();
		u64 ArraySize;
		Obj->GetCurrentChunk()->ReadArray(ArraySize);
		{
			for (u64 i = 0; i < ArraySize; ++i) {
				SStatSectionData&& temp = SStatSectionData();
				temp.load(Obj);
				Value.emplace_back(temp);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

};