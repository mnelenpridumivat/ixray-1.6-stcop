////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_abstract_registry_inline.h
//	Created 	: 30.06.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife abstract registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "game_news.h"
#include "encyclopedia_article_defs.h"
#include "character_info_defs.h"
#include "relation_registry_defs.h"
#include "InfoPortionDefs.h"
#include "_stl_extensions.h"
#include "map_location_defs.h"

namespace SaveSystemDefined {
	
	template<typename K, typename V>
	void Save(CSaveObjectSave* Obj, K Key, const V& Value) {
		VERIFY2(false, "Not implemented for such template arguments!");
	}

	template<typename K, typename V>
	void Load(CSaveObjectLoad* Obj, K& Key, V& Value) {
		VERIFY2(false, "Not implemented for such template arguments!");
	}

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
				GAME_NEWS_DATA&& temp = GAME_NEWS_DATA();
				temp.load(Obj);
				Value.emplace_back(temp);
			}
		}
		Obj->GetCurrentChunk()->EndArray();
	}

};

#define TEMPLATE_SPECIALIZATION template <typename _index_type, typename _data_type>
#define CSALifeAbstractRegistry CALifeAbstractRegistry<_index_type,_data_type>

TEMPLATE_SPECIALIZATION
IC	CSALifeAbstractRegistry::CALifeAbstractRegistry	()
{
}

TEMPLATE_SPECIALIZATION
CSALifeAbstractRegistry::~CALifeAbstractRegistry		()
{
	delete_data		(m_objects);
}

TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry::save					(IWriter &memory_stream)
{
	save_data		(m_objects,memory_stream);
}

TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry ::load					(IReader &file_stream)
{
	load_data		(m_objects,file_stream);
}

TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry::save(CSaveObjectSave* Object) const
{
	Object->GetCurrentChunk()->WriteArray(m_objects.size());
	{
		for (const auto& elem : m_objects) {
			Object->BeginChunk("CSALifeAbstractRegistry::elem");
			{
				SaveSystemDefined::Save<_index_type, _data_type>(Object, elem.first, elem.second);
			}
			Object->EndChunk();
		}
	}
	Object->GetCurrentChunk()->EndArray();
}

TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry::load(CSaveObjectLoad* Object)
{
	u64 ArraySize;
	Object->GetCurrentChunk()->ReadArray(ArraySize);
	{
		for (u64 i = 0; i < ArraySize; ++i) {		
			std::pair< _index_type, _data_type>&& temp = std::pair< _index_type, _data_type>();
			Object->BeginChunk("CSALifeAbstractRegistry::elem");
			{
				SaveSystemDefined::Load<_index_type, _data_type>(Object, temp.first, temp.second);
			}
			Object->EndChunk();
			m_objects.emplace(temp);
		}
	}
	Object->GetCurrentChunk()->EndArray();
}

TEMPLATE_SPECIALIZATION
IC	const typename CSALifeAbstractRegistry::OBJECT_REGISTRY &CSALifeAbstractRegistry::objects	() const
{
	return			(m_objects);
}

TEMPLATE_SPECIALIZATION
IC	void CSALifeAbstractRegistry::add				(const _index_type &index, _data_type &data, bool no_assert)
{
	const_iterator	I = objects().find(index);
	if (I != objects().end()) {
		THROW2		(no_assert,"Specified object has been already found in the specified registry!");
		return;
	}
	m_objects.insert(std::make_pair(index,data));
}

TEMPLATE_SPECIALIZATION
IC	void CSALifeAbstractRegistry::remove			(const _index_type &index, bool no_assert)
{
	iterator		I = m_objects.find(index);
	if (I == objects().end()) {
		THROW2		(no_assert,"Specified object hasn't been found in the specified registry!");
		return;
	}
	m_objects.erase	(I);
}

TEMPLATE_SPECIALIZATION
IC	_data_type *CSALifeAbstractRegistry::object	(const _index_type &index, bool no_assert)
{
	iterator		I = m_objects.find(index);
	if (I == objects().end()) {
		THROW2		(no_assert,"Specified object hasn't been found in the specified registry!");
		return		(0);
	}
	return			(&(*I).second);
}

#undef TEMPLATE_SPECIALIZATION