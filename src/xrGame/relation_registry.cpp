//////////////////////////////////////////////////////////////////////////
// relation_registry.cpp:	реестр для хранения данных об отношении персонажа к 
//							другим персонажам
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "relation_registry.h"
#include "alife_registry_wrappers.h"

#include "character_community.h"
#include "character_reputation.h"
#include "character_rank.h"

#include "alife_object_registry.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "../xrScripts/script_engine.h"


//////////////////////////////////////////////////////////////////////////

SRelation::SRelation()
{
	m_iGoodwill = NEUTRAL_GOODWILL;
}

SRelation::~SRelation()
{
}

//////////////////////////////////////////////////////////////////////////

void RELATION_DATA::clear	()
{
	personal.clear();
	communities.clear();
}

void RELATION_DATA::load (IReader& stream)
{
	load_data(personal, stream);
	load_data(communities, stream);
}

void RELATION_DATA::save (IWriter& stream)
{
	save_data(personal, stream);
	save_data(communities, stream);
}

void RELATION_DATA::load(CSaveObjectLoad* Object)
{
	Object->BeginChunk("RELATION_DATA");
	{
		Object->BeginChunk("RELATION_DATA::personal");
		{
			u64 ArraySize;
			Object->GetCurrentChunk()->ReadArray(ArraySize);
			for (u64 i = 0; i < ArraySize; ++i) {
				Object->BeginChunk("RELATION_DATA::personal::elem");
				{
					std::pair<PERSONAL_RELATION_MAP::key_type, PERSONAL_RELATION_MAP::mapped_type>&& temp = std::pair<PERSONAL_RELATION_MAP::key_type, PERSONAL_RELATION_MAP::mapped_type>();
					Object->GetCurrentChunk()->r_u16(temp.first);
					s32 Goodwill;
					Object->GetCurrentChunk()->r_s32(Goodwill);
					temp.second.SetGoodwill(Goodwill);
					personal.emplace(temp);
				}
				Object->EndChunk();
			}
			Object->GetCurrentChunk()->EndArray();
		}
		Object->EndChunk();
		Object->BeginChunk("RELATION_DATA::communities");
		{
			u64 ArraySize;
			Object->GetCurrentChunk()->ReadArray(ArraySize);
			for (u64 i = 0; i < ArraySize; ++i) {
				Object->BeginChunk("RELATION_DATA::communities::elem");
				{
					std::pair<COMMUNITY_RELATION_MAP::key_type, COMMUNITY_RELATION_MAP::mapped_type>&& temp = std::pair<COMMUNITY_RELATION_MAP::key_type, COMMUNITY_RELATION_MAP::mapped_type>();
					s32 CommunityIndex;
					Object->GetCurrentChunk()->r_s32(CommunityIndex);
					temp.first = CommunityIndex;
					s32 Goodwill;
					Object->GetCurrentChunk()->r_s32(Goodwill);
					temp.second.SetGoodwill(Goodwill);
					communities.emplace(temp);
				}
				Object->EndChunk();
			}
			Object->GetCurrentChunk()->EndArray();
		}
		Object->EndChunk();
	}
	Object->EndChunk();
}

void RELATION_DATA::save(CSaveObjectSave* Object) const
{
	Object->BeginChunk("RELATION_DATA");
	{
		Object->BeginChunk("RELATION_DATA::personal");
		{
			Object->GetCurrentChunk()->WriteArray(personal.size());
			for (const auto& elem : personal) {
				Object->BeginChunk("RELATION_DATA::personal::elem");
				{
					Object->GetCurrentChunk()->w_u16(elem.first);
					s32 Goodwill = elem.second.Goodwill();
					Object->GetCurrentChunk()->w_s32(Goodwill);
				}
				Object->EndChunk();
			}
			Object->GetCurrentChunk()->EndArray();
		}
		Object->EndChunk();
		Object->BeginChunk("RELATION_DATA::communities");
		{
			Object->GetCurrentChunk()->WriteArray(communities.size());
			for (const auto& elem : communities) {
				Object->BeginChunk("RELATION_DATA::communities::elem");
				{
					s32 CommunityIndex = elem.first;
					Object->GetCurrentChunk()->w_s32(CommunityIndex);
					s32 Goodwill = elem.second.Goodwill();
					Object->GetCurrentChunk()->w_s32(Goodwill);
				}
				Object->EndChunk();
			}
			Object->GetCurrentChunk()->EndArray();
		}
		Object->EndChunk();
	}
	Object->EndChunk();
}

//////////////////////////////////////////////////////////////////////////

RELATION_REGISTRY::RELATION_MAP_SPOTS::RELATION_MAP_SPOTS()
{
	spot_names[ALife::eRelationTypeFriend]		= "friend_location";
	spot_names[ALife::eRelationTypeNeutral]		= "neutral_location";
	spot_names[ALife::eRelationTypeEnemy]		= "enemy_location";
	spot_names[ALife::eRelationTypeWorstEnemy]	= "enemy_location";
	spot_names[ALife::eRelationTypeLast]		= "neutral_location";
}
//////////////////////////////////////////////////////////////////////////

CRelationRegistryWrapper*					RELATION_REGISTRY::m_relation_registry	= nullptr;
RELATION_REGISTRY::FIGHT_VECTOR*			RELATION_REGISTRY::m_fight_registry		= nullptr;
RELATION_REGISTRY::RELATION_MAP_SPOTS*		RELATION_REGISTRY::m_spot_names			= nullptr;


//////////////////////////////////////////////////////////////////////////


RELATION_REGISTRY::RELATION_REGISTRY  ()
{
}

RELATION_REGISTRY::~RELATION_REGISTRY ()
{
}

//////////////////////////////////////////////////////////////////////////

extern void load_attack_goodwill();
extern bool IsGameTypeSingle	();
CRelationRegistryWrapper& RELATION_REGISTRY::relation_registry()
{
	if(!m_relation_registry){
		m_relation_registry = new CRelationRegistryWrapper();
		load_attack_goodwill();
	}

	return *m_relation_registry;
}


RELATION_REGISTRY::FIGHT_VECTOR& RELATION_REGISTRY::fight_registry()
{
	if(!m_fight_registry)
		m_fight_registry = new FIGHT_VECTOR();

	return *m_fight_registry;
}

void RELATION_REGISTRY::clear_relation_registry()
{
	xr_delete(m_relation_registry);
	xr_delete(m_fight_registry);
	xr_delete(m_spot_names);
}

const shared_str& RELATION_REGISTRY::GetSpotName(ALife::ERelationType& type)
{
	if(!m_spot_names)
		m_spot_names = new RELATION_MAP_SPOTS();
	return m_spot_names->GetSpotName(type);
}

//////////////////////////////////////////////////////////////////////////

void RELATION_REGISTRY::ClearRelations	(u16 person_id)
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(person_id);
	if(relation_data)
	{
		relation_registry().registry().objects(person_id).clear();
	}
}



//////////////////////////////////////////////////////////////////////////
CHARACTER_GOODWILL	 RELATION_REGISTRY::GetGoodwill			(u16 from, u16 to) const 
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(from);

	if(relation_data)
	{
		PERSONAL_RELATION_MAP::const_iterator it = relation_data->personal.find(to);
		if(relation_data->personal.end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
	//если отношение еще не задано, то возвращаем нейтральное
	return NEUTRAL_GOODWILL;
}

void RELATION_REGISTRY::SetGoodwill 	(u16 from, u16 to, CHARACTER_GOODWILL goodwill)
{
	RELATION_DATA& relation_data = relation_registry().registry().objects(from);

	static Ivector2 gw_limits		= pSettings->r_ivector2(ACTIONS_POINTS_SECT, "personal_goodwill_limits");
	clamp							(goodwill, gw_limits.x, gw_limits.y);

	relation_data.personal[to].SetGoodwill(goodwill);
}

void RELATION_REGISTRY::ForceSetGoodwill 	(u16 from, u16 to, CHARACTER_GOODWILL goodwill)
{
	RELATION_DATA& relation_data = relation_registry().registry().objects(from);

	CSE_ALifeTraderAbstract* from_obj	= smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(from));
	CSE_ALifeTraderAbstract* to_obj		= smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(to));
	
	if (!from_obj||!to_obj){
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"RELATION_REGISTRY::ForceSetGoodwill  : cannot convert obj to CSE_ALifeTraderAbstract!");
		return;
	}	
	CHARACTER_GOODWILL community_to_obj_goodwill		= GetCommunityGoodwill	(from_obj->Community(), to					);
	CHARACTER_GOODWILL community_to_community_goodwill	= GetCommunityRelation	(from_obj->Community(), to_obj->Community()	);
	
	relation_data.personal[to].SetGoodwill(goodwill - community_to_obj_goodwill - community_to_community_goodwill);
}


void RELATION_REGISTRY::ChangeGoodwill 	(u16 from, u16 to, CHARACTER_GOODWILL delta_goodwill)
{
	CHARACTER_GOODWILL new_goodwill		= GetGoodwill(from, to)+ delta_goodwill;
	SetGoodwill							(from, to, new_goodwill);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_GOODWILL	 RELATION_REGISTRY::GetCommunityGoodwill (CHARACTER_COMMUNITY_INDEX from_community, u16 to_character) const 
{
	const RELATION_DATA* relation_data = relation_registry().registry().objects_ptr(to_character);

	if(relation_data)
	{
		COMMUNITY_RELATION_MAP::const_iterator it = relation_data->communities.find(from_community);
		if(relation_data->communities.end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
	//если отношение еще не задано, то возвращаем нейтральное
	return NEUTRAL_GOODWILL;
}

void RELATION_REGISTRY::SetCommunityGoodwill 	(CHARACTER_COMMUNITY_INDEX from_community, u16 to_character, CHARACTER_GOODWILL goodwill)
{
	static Ivector2 gw_limits		= pSettings->r_ivector2(ACTIONS_POINTS_SECT, "community_goodwill_limits");
	clamp							(goodwill, gw_limits.x, gw_limits.y);
	RELATION_DATA& relation_data	= relation_registry().registry().objects(to_character);

	relation_data.communities[from_community].SetGoodwill(goodwill);
}

void RELATION_REGISTRY::ChangeCommunityGoodwill (CHARACTER_COMMUNITY_INDEX from_community, u16 to_character, CHARACTER_GOODWILL delta_goodwill)
{
	CHARACTER_GOODWILL gw = GetCommunityGoodwill(from_community, to_character)+ delta_goodwill;
	SetCommunityGoodwill	(from_community, to_character, gw);
}
//////////////////////////////////////////////////////////////////////////

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetCommunityRelation		(CHARACTER_COMMUNITY_INDEX index1, CHARACTER_COMMUNITY_INDEX index2) const
{
	return CHARACTER_COMMUNITY::relation(index1, index2);
}

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetRankRelation				(CHARACTER_RANK_VALUE rank1, CHARACTER_RANK_VALUE rank2) const
{
	CHARACTER_RANK rank_from, rank_to;
	rank_from.set(rank1);
	rank_to.set(rank2);
	return CHARACTER_RANK::relation(rank_from.index(), rank_to.index());
}

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetReputationRelation		(CHARACTER_REPUTATION_VALUE rep1, CHARACTER_REPUTATION_VALUE rep2) const
{
	CHARACTER_REPUTATION rep_from, rep_to;
	rep_from.set(rep1);
	rep_to.set(rep2);
	return CHARACTER_REPUTATION::relation(rep_from.index(), rep_to.index());
}

//////////////////////////////////////////////////////////////////////////

void RELATION_REGISTRY::SetCommunityRelation( CHARACTER_COMMUNITY_INDEX index1, CHARACTER_COMMUNITY_INDEX index2, CHARACTER_GOODWILL goodwill )
{
	CHARACTER_COMMUNITY::set_relation( index1, index2, goodwill );
}
