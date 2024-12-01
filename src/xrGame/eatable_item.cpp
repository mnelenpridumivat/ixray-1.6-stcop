////////////////////////////////////////////////////////////////////////////
//	Module 		: eatable_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Yuri Dobronravin
//	Description : Eatable item
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "eatable_item.h"
#include "xrMessages.h"
#include "physic_item.h"
#include "Level.h"
#include "entity_alive.h"
#include "EntityCondition.h"
#include "InventoryOwner.h"
#include "UIGameCustom.h"
#include "ui/UIActorMenu.h"
#include "HUDAnimItem.h"
#include "../xrPhysics/ElevatorState.h"
#include "player_hud.h"
#include <CustomDetector.h>
#include "../xrEngine/Effector.h"
#include "Actor.h"
#include "Inventory.h"
#include "ActorEffector.h"
#include "CustomOutfit.h"

extern bool g_block_all_except_movement;

CEatableItem::CEatableItem()
{
	use_cam_effector = nullptr;
	anim_sect = nullptr;
	m_iAnimHandsCnt = 1;
	m_iAnimLength = 0;

	m_physic_item = nullptr;
	m_fWeightFull = 0;
	m_fWeightEmpty = 0;

	m_iMaxUses = 1;
	m_iRemainingUses = 1;
	m_bRemoveAfterUse = true;
	m_bConsumeChargeOnUse = true;
	m_bUnlimited = false;
	m_fEffectorIntensity = 1.0f;
}

CEatableItem::~CEatableItem()
{
}

DLL_Pure *CEatableItem::_construct	()
{
	m_physic_item	= smart_cast<CPhysicItem*>(this);
	return			(inherited::_construct());
}

void CEatableItem::Load(LPCSTR section)
{
	inherited::Load(section);

	bUseHUDAnim = (pSettings->line_exist(section, "anm_use"));

	if (pSettings->line_exist(section, "eat_portions_num"))
	{
		m_iMaxUses = pSettings->r_s32(section, "eat_portions_num");
	}
	else
	{
		m_iMaxUses = READ_IF_EXISTS(pSettings, r_u8, section, "max_uses", 1);
	}

	UseText = READ_IF_EXISTS(pSettings, r_string, section, "use_text", "st_use");

	if (m_iMaxUses < 1)
		m_iMaxUses = 1;

	m_iPortionsMarker = m_iMaxUses;

	m_bRemoveAfterUse = READ_IF_EXISTS( pSettings, r_bool, section, "remove_after_use", TRUE );
	m_eat_condition = READ_IF_EXISTS(pSettings, r_float, section, "eat_condition", 1);
	m_bConsumeChargeOnUse = READ_IF_EXISTS(pSettings, r_bool, section, "consume_charge_on_use", TRUE);
	m_fWeightFull = m_weight;
	m_fWeightEmpty = READ_IF_EXISTS(pSettings, r_float, section, "empty_weight", 0.0f);

	anim_sect = READ_IF_EXISTS(pSettings, r_string, section, "hud_section", nullptr);
	m_iRemainingUses = m_iMaxUses = READ_IF_EXISTS(pSettings, r_u32, section, "eat_portions_num", 1);
	m_bUnlimited = READ_IF_EXISTS(pSettings, r_bool, section, "unlimited_usage", false);
	m_fEffectorIntensity = READ_IF_EXISTS(pSettings, r_float, section, "cam_effector_intensity", 1.0f);
	use_cam_effector = READ_IF_EXISTS(pSettings, r_string, section, "use_cam_effector", nullptr);
}


void CEatableItem::load(IReader& packet)
{
	inherited::load(packet);
	load_data(m_iRemainingUses, packet);
	m_iPortionsMarker = packet.r_u8();

	if (g_block_all_except_movement)
		g_block_all_except_movement = false;

	if (!g_actor_allow_ladder)
		g_actor_allow_ladder = true;
}

void CEatableItem::UpdateInRuck(CActor* actor)
{
	UpdateUseAnim(actor);

	/*if (GameConstants::GetFoodIrradiation())
	{
		float m_radia_hit = CurrentGameUI()->get_zone_cur_power(ALife::eHitTypeRadiation);
		float irradiation_coef = ((m_fIrradiationCoef + m_radia_hit) / 64) * Device.fTimeDelta;

		if (m_radia_hit > m_fIrradiationZonePower)
			m_fRadioactivity += irradiation_coef;

		clamp(m_fRadioactivity, 0.0f, 1.0f);
	}

	if (GameConstants::GetFoodRotting() && GameConstants::GetActorIntoxication())
	{
		float rotten_coef = (m_fFoodRottingCoef / 128) * Device.fTimeDelta;
		static float spoliage = m_fSpoliage;

		if (spoliage < 1.0f)
			spoliage += rotten_coef;

		if (spoliage > 0.0f)
			m_fSpoliage = smoothstep(0.75f, 1.0f, spoliage);

		clamp(m_fFoodRottingCoef, 0.0f, 1.0f);
	}*/
}

void CEatableItem::save(NET_Packet& packet)
{
	inherited::save(packet);
	save_data(m_iRemainingUses, packet);
	packet.w_u8((u8)m_iPortionsMarker);
}

BOOL CEatableItem::net_Spawn(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC)) return FALSE;
	return TRUE;
}

bool CEatableItem::Useful() const
{
	if(!inherited::Useful()) return false;

	//проверить не все ли еще съедено
	if (GetRemainingUses() == 0 && !m_bUnlimited && CanDelete()) return false;

	return true;
}

void CEatableItem::OnH_A_Independent() 
{
	inherited::OnH_A_Independent();
	if(!Useful()) {
		if (object().Local() && OnServer())	object().DestroyObject	();
	}	
}

void CEatableItem::OnH_B_Independent(bool just_before_destroy)
{
	if(!Useful()) 
	{
		object().setVisible(FALSE);
		object().setEnabled(FALSE);
		if (m_physic_item)
			m_physic_item->m_ready_to_destroy	= true;
	}
	inherited::OnH_B_Independent(just_before_destroy);
}

bool CEatableItem::UseBy (CEntityAlive* entity_alive)
{
	SMedicineInfluenceValues	V;
	V.Load						(m_physic_item->cNameSect());

	CInventoryOwner* IO	= smart_cast<CInventoryOwner*>(entity_alive);
	R_ASSERT		(IO);
	R_ASSERT		(m_pInventory==IO->m_inventory);
	R_ASSERT		(object().H_Parent()->ID()==entity_alive->ID());

	entity_alive->conditions().ApplyInfluence(V, m_physic_item->cNameSect());

	for(u8 i = 0; i<(u8)eBoostMaxCount; i++)
	{
		if(pSettings->line_exist(m_physic_item->cNameSect().c_str(), ef_boosters_section_names[i]))
		{
			SBooster B;
			B.Load(m_physic_item->cNameSect(), (EBoostParams)i);
			entity_alive->conditions().ApplyBooster(B, m_physic_item->cNameSect());
		}
	}

	if (m_iPortionsMarker > 0)
		m_iPortionsMarker -= m_eat_condition;
	else
		m_iPortionsMarker = 0;

	if (bUseHUDAnim)
	{
		CHUDAnimItem::PlayHudAnim(m_section_id.c_str(), "anm_use");
	}

	if (m_iRemainingUses != u8(-1) && !m_bUnlimited)
	{
		if (m_iRemainingUses > 0)
			--(m_iRemainingUses);
		else
			m_iRemainingUses = 0;
	}

	if (m_iRemainingUses > 1 && CurrentGameUI()->ActorMenu().IsShown() && CurrentGameUI()->ActorMenu().GetMenuMode() != mmDeadBodySearch) {
		CurrentGameUI()->ActorMenu().RefreshConsumableCells();
	}

	return true;
}

u32 CEatableItem::Cost() const
{
	u32 res = inherited::Cost();
	int percent = (m_iRemainingUses * 100) / m_iMaxUses;

	res = (res * percent) / 100;

	return res;
}

float CEatableItem::Weight() const
{
	float res = inherited::Weight();

	if (IsUsingCondition())
	{
		float net_weight = m_fWeightFull - m_fWeightEmpty;
		float use_weight = m_iMaxUses > 0 ? (net_weight / m_iMaxUses) : 0.f;

		res = m_fWeightEmpty + (GetRemainingUses() * use_weight);
	}

	return res;
}

bool CEatableItem::CheckInventoryIconItemSimilarity(CInventoryItem* other)
{
	if (!inherited::CheckInventoryIconItemSimilarity(other))
	{
		return false;
	}
	auto eatable = smart_cast<CEatableItem*>(other);
	VERIFY(eatable);
	return eatable->GetRemainingUses() == GetRemainingUses();
}

void CEatableItem::UpdateUseAnim(CActor* actor)
{
	if (!m_bHasAnimation) return;

	CCustomDetector* pDet = smart_cast<CCustomDetector*>(actor->inventory().ItemFromSlot(DETECTOR_SLOT));
	bool IsActorAlive = g_pGamePersistent->GetActorAliveStatus();

	if (m_bItmStartAnim && actor->inventory().GetActiveSlot() == NO_ACTIVE_SLOT && (!pDet || pDet->IsHidden()))
		StartAnimation();

	if (!IsActorAlive)
	{
		m_using_sound.stop();

		//if (pSettings->line_exist(anim_sect, "hud_fov") && last_hud_fov > 0.0f)
		//	psHUD_FOV_def = last_hud_fov;
	}

	if (m_bActivated)
	{
		if (m_iAnimLength <= Device.dwTimeGlobal || !IsActorAlive)
		{
			actor->SetWeaponHideState(INV_STATE_BLOCK_ALL, false);

			m_iAnimLength = Device.dwTimeGlobal;
			m_bActivated = false;
			g_block_all_except_movement = false;
			g_actor_allow_ladder = true;
			actor->m_bActionAnimInProcess = false;

			//if (pSettings->line_exist(anim_sect, "hud_fov") && last_hud_fov > 0.0f)
			//	psHUD_FOV_def = last_hud_fov;

			CEffectorCam* effector = actor->Cameras().GetCamEffector(static_cast<ECamEffectorType>(effUseItem));

			if (effector)
				RemoveEffector(actor, effUseItem);

			//ps_ssfx_wpn_dof_1 = GameConstants::GetSSFX_DefaultDoF();
			//ps_ssfx_wpn_dof_2 = GameConstants::GetSSFX_DefaultDoF().z;

			if (IsActorAlive)
				actor->inventory().Eat(this);
		}
	}
}

void CEatableItem::HideWeapon()
{
	CEffectorCam* effector = Actor()->Cameras().GetCamEffector(static_cast<ECamEffectorType>(effUseItem));
	CCustomDetector* pDet = smart_cast<CCustomDetector*>(Actor()->inventory().ItemFromSlot(DETECTOR_SLOT));

	Actor()->SetWeaponHideState(INV_STATE_BLOCK_ALL, true);

	if (pDet)
		pDet->HideDetector(true);

	m_bItmStartAnim = true;

	if (!Actor()->inventory_disabled())
		CurrentGameUI()->HideActorMenu();
}

void CEatableItem::StartAnimation()
{
	m_bActivated = true;

	if (pSettings->line_exist(anim_sect, "single_handed_anim"))
		m_iAnimHandsCnt = pSettings->r_u32(anim_sect, "single_handed_anim");

	m_bItmStartAnim = false;
	g_block_all_except_movement = true;
	g_actor_allow_ladder = false;
	Actor()->m_bActionAnimInProcess = true;

	CCustomOutfit* cur_outfit = Actor()->GetOutfit();

	if (pSettings->line_exist(anim_sect, "anm_use"))
	{
		string128 anim_name{};
		xr_strconcat(anim_name, "anm_use", (cur_outfit && cur_outfit->m_bHasLSS) ? "_exo" : (m_iRemainingUses == 1) ? "_last" : "");

		LPCSTR attach_visual = READ_IF_EXISTS(pSettings, r_string, anim_sect, (cur_outfit && cur_outfit->m_bHasLSS) ? "item_visual_exo" : "item_visual", nullptr);

		if (pSettings->line_exist(anim_sect, anim_name))
		{
			g_player_hud->script_anim_play(m_iAnimHandsCnt, anim_sect, anim_name, false, 1.0f, attach_visual);
			m_iAnimLength = Device.dwTimeGlobal + g_player_hud->motion_length_script(anim_sect, anim_name, 1.0f);
		}
		else
		{
			g_player_hud->script_anim_play(m_iAnimHandsCnt, anim_sect, "anm_use", false, 1.0f, attach_visual);
			m_iAnimLength = Device.dwTimeGlobal + g_player_hud->motion_length_script(anim_sect, "anm_use", 1.0f);
		}

		//if (pSettings->line_exist(anim_sect, "hud_fov"))
		//{
		//	last_hud_fov = psHUD_FOV_def;
		//	psHUD_FOV_def = pSettings->r_float(anim_sect, "hud_fov");
		//}

		//ps_ssfx_wpn_dof_1 = GameConstants::GetSSFX_FocusDoF();
		//ps_ssfx_wpn_dof_2 = GameConstants::GetSSFX_FocusDoF().z;
	}

	CEffectorCam* effector = Actor()->Cameras().GetCamEffector(static_cast<ECamEffectorType>(effUseItem));

	if (!effector && use_cam_effector != nullptr)
		AddEffector(Actor(), effUseItem, use_cam_effector, m_fEffectorIntensity);

	if (pSettings->line_exist(anim_sect, "snd_using"))
	{
		if (m_using_sound._feedback())
			m_using_sound.stop();

		string128 snd_var_name{};
		shared_str snd_name{};

		xr_strconcat(snd_var_name, "snd_using", (cur_outfit && cur_outfit->m_bHasLSS) ? "_exo" : (m_iRemainingUses == 1) ? "_last" : "");

		if (pSettings->line_exist(anim_sect, snd_var_name))
			snd_name = pSettings->r_string(anim_sect, snd_var_name);
		else
			snd_name = pSettings->r_string(anim_sect, "snd_using");

		m_using_sound.create(snd_name.c_str(), st_Effect, sg_SourceType);
		m_using_sound.play(nullptr, sm_2D);
	}
}


using namespace luabind;

#pragma optimize("s",on)
void CEatableItem::script_register(lua_State *L)
{
	module(L)
		[
			class_<CEatableItem>("CEatableItem")
			.def("Empty", &CEatableItem::Empty)
			.def("CanDelete", &CEatableItem::CanDelete)
			.def("GetMaxUses", &CEatableItem::GetMaxUses)
			.def("GetRemainingUses", &CEatableItem::GetRemainingUses)
			.def("SetRemainingUses", &CEatableItem::SetRemainingUses)

			.def_readwrite("m_bRemoveAfterUse", &CEatableItem::m_bRemoveAfterUse)
			.def_readwrite("m_fWeightFull", &CEatableItem::m_fWeightFull)
			.def_readwrite("m_fWeightEmpty", &CEatableItem::m_fWeightEmpty)

			.def("Weight", &CEatableItem::Weight)
			.def("Cost", &CEatableItem::Cost)
		];
}

SCRIPT_EXPORT1(CEatableItem);