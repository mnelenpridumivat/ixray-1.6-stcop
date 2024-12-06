#include "stdafx.h"
#include "pch_script.h"

#include "Flamethrower.h"

#ifdef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC

void	CFlamethrower::OnMagazineEmpty(){}
void	CFlamethrower::switch2_Idle(){}
void	CFlamethrower::switch2_Fire(){}
void	CFlamethrower::switch2_Empty() {}
void	CFlamethrower::switch2_Reload(){}
void	CFlamethrower::switch2_Hiding(){}
void	CFlamethrower::switch2_Hidden(){}
void	CFlamethrower::switch2_Showing() {}
void    CFlamethrower::switch2_Unmis() {}
void	CFlamethrower::OnShot() {}
void	CFlamethrower::StopShooting(){}
void	CFlamethrower::OnEmptyClick(){}
void	CFlamethrower::OnAnimationEnd(u32 state) {}
void	CFlamethrower::OnStateSwitch(u32 S) {}
void	CFlamethrower::UpdateSounds() {}
bool	CFlamethrower::TryReload() { return false; }
void	CFlamethrower::ReloadMagazine() {}
void	CFlamethrower::state_FireCharge(float dt) {}
void	CFlamethrower::state_Fire(float dt){}
void	CFlamethrower::state_Idle(float dt){}
void	CFlamethrower::state_MagEmpty(float dt) {}
void	CFlamethrower::state_Misfire(float dt) {}
CFlamethrower::CFlamethrower(ESoundTypes eSoundType) {}
CFlamethrower::~CFlamethrower() {}
void	CFlamethrower::Load(LPCSTR section) {}
bool    CFlamethrower::UseScopeTexture() { return false; }
void	CFlamethrower::SetDefaults() {}
void	CFlamethrower::FireStart() {}
void	CFlamethrower::FireEnd() {}
void	CFlamethrower::Reload() {}
void	CFlamethrower::UpdateCL() {}
void	CFlamethrower::net_Destroy() {}
void	CFlamethrower::net_Export(NET_Packet& P){}
void	CFlamethrower::net_Import(NET_Packet& P){}
void	CFlamethrower::OnH_A_Chield() {}
bool	CFlamethrower::Attach(PIItem pIItem, bool b_send_event) { return false; }
bool	CFlamethrower::Detach(const char* item_section_name, bool b_spawn_item) { return false; }
bool	CFlamethrower::DetachScope(const char* item_section_name, bool b_spawn_item) { return false; }
bool	CFlamethrower::CanAttach(PIItem pIItem) { return false; }
bool	CFlamethrower::CanDetach(const char* item_section_name) { return false; }
void	CFlamethrower::InitAddons() {}
bool	CFlamethrower::Action(u16 cmd, u32 flags) { return false; }
bool	CFlamethrower::IsAmmoAvailable() { return false; }
void	CFlamethrower::UnloadMagazine(bool spawn_ammo) {}
int     CFlamethrower::CheckAmmoBeforeReload(u8& v_ammoType) { return 0; }
void	CFlamethrower::OnMotionMark(u32 state, const motion_marks& M) {}
bool	CFlamethrower::GetBriefInfo(II_BriefInfo& info) { return false; }
BOOL	CFlamethrower::IsMisfire() const { return false; }
void	CFlamethrower::OnZoomIn() {}
void	CFlamethrower::OnZoomOut() {}
void	CFlamethrower::save(NET_Packet& output_packet) {}
void	CFlamethrower::load(IReader& input_packet) {}
void	CFlamethrower::Save(CSaveObjectSave* Object) const {}
void	CFlamethrower::Load(CSaveObjectLoad* Object) {}
void	CFlamethrower::SpawnFuelCanister(float Condition, LPCSTR ammoSect, u32 ParentID) {}
bool	CFlamethrower::install_upgrade_impl(LPCSTR section, bool test) { return false; }
void	CFlamethrower::PlayAnimShow() {}
void	CFlamethrower::PlayAnimHide() {}
void	CFlamethrower::PlayAnimReload() {}
void	CFlamethrower::PlayAnimIdle() {}
void	CFlamethrower::PlayAnimShoot() {}
void	CFlamethrower::PlayReloadSound() {}
void	CFlamethrower::PlayAnimBore() {}
void	CFlamethrower::PlayAnimIdleSprint() {}
void	CFlamethrower::PlayAnimIdleMoving() {}
void    CFlamethrower::SetAnimFlag(u32 flag, LPCSTR anim_name) {}
bool CFlamethrower::WeaponSoundExist(LPCSTR section, LPCSTR sound_name, bool log) const { return false; }
float	CFlamethrower::GetWeaponDeterioration() { return 0.0f; }
void	CFlamethrower::FireBullet(const Fvector& pos,
	const Fvector& dir,
	float fire_disp,
	const CCartridge& cartridge,
	u16 parent_id,
	u16 weapon_id,
	bool send_hit) {}
	
#else
#include "actor.h"
#include "../xrParticles/ParticlesObject.h"
#include "scope.h"
#include "silencer.h"
#include "GrenadeLauncher.h"
//#include "LaserDesignator.h"
//#include "TacticalTorch.h"
#include "inventory.h"
#include "InventoryOwner.h"
#include "xrserver_objects_alife_items.h"
#include "ActorEffector.h"
#include "EffectorZoomInertion.h"
#include "../xrEngine/xr_level_controller.h"
#include "UIGameCustom.h"
#include "object_broker.h"
#include "../xrEngine/string_table.h"
#include "MPPlayersBag.h"
#include "../xrUI/UIXmlInit.h"
#include "../xrUI/Widgets/UIStatic.h"
#include "game_object_space.h"
#include "../xrScripts/script_callback_ex.h"
#include "script_game_object.h"
//#include "AdvancedXrayGameConstants.h"
#include "FlameCanister.h"
#include "FlamethrowerTraceCollision.h"
#include "ai_object_location.h"

ENGINE_API bool	g_dedicated_server;
ENGINE_API  extern float psHUD_FOV;
ENGINE_API  extern float psHUD_FOV_def;

//CUIXml*				pWpnScopeXml = NULL;

CFlamethrower::CFlamethrower(ESoundTypes eSoundType) : CWeapon()
{
	m_eSoundShow = static_cast<ESoundTypes>(SOUND_TYPE_ITEM_TAKING | eSoundType);
	m_eSoundHide = static_cast<ESoundTypes>(SOUND_TYPE_ITEM_HIDING | eSoundType);
	m_eSoundShot = static_cast<ESoundTypes>(SOUND_TYPE_WEAPON_SHOOTING | eSoundType);
	m_eSoundEmptyClick = static_cast<ESoundTypes>(SOUND_TYPE_WEAPON_EMPTY_CLICKING | eSoundType);
	m_eSoundReload = static_cast<ESoundTypes>(SOUND_TYPE_WEAPON_RECHARGING | eSoundType);
	m_eSoundClose = static_cast<ESoundTypes>(SOUND_TYPE_WEAPON_RECHARGING);
	m_sounds_enabled = true;

	psWpnAnimsFlag = { 0 };

	m_bFireSingleShot = false;
	m_fOldBulletSpeed = 0;
	bullet_cnt = 0;
	m_bLockType = false;
	m_bAutoreloadEnabled = READ_IF_EXISTS(pAdvancedSettings, r_bool, "gameplay", "autoreload_enabled", true);
	m_bNeedBulletInGun = false;
	m_opened = false;
	m_bUseFiremodeChangeAnim = true;
	bHasBulletsToHide = false;

	m_sSndShotCurrent = nullptr;

	TraceManager = xr_new<FlamethrowerTrace::CManager>(this);
}

CFlamethrower::~CFlamethrower()
{
	// sounds
	xr_delete(TraceManager);
}


void CFlamethrower::net_Destroy()
{
	inherited::net_Destroy();
}

void CFlamethrower::SetAnimFlag(u32 flag, LPCSTR anim_name)
{
	if (pSettings->line_exist(hud_sect, anim_name))
		psWpnAnimsFlag.set(flag, TRUE);
	else
		psWpnAnimsFlag.set(flag, FALSE);
}

void CFlamethrower::Load(LPCSTR section)
{
	inherited::Load(section);

	// ��������� ������� ��������
	SetAnimFlag(ANM_SHOW_EMPTY, "anm_show_empty");
	SetAnimFlag(ANM_HIDE_EMPTY, "anm_hide_empty");
	SetAnimFlag(ANM_IDLE_EMPTY, "anm_idle_empty");
	SetAnimFlag(ANM_AIM_EMPTY, "anm_idle_aim_empty");
	SetAnimFlag(ANM_BORE_EMPTY, "anm_bore_empty");
	SetAnimFlag(ANM_SHOT_EMPTY, "anm_shot_l");
	SetAnimFlag(ANM_SPRINT_EMPTY, "anm_idle_sprint_empty");
	SetAnimFlag(ANM_MOVING_EMPTY, "anm_idle_moving_empty");
	SetAnimFlag(ANM_RELOAD_EMPTY, "anm_reload_empty");
	SetAnimFlag(ANM_MISFIRE, "anm_reload_misfire");
	SetAnimFlag(ANM_SHOT_AIM, "anm_shots_when_aim");

	// Sounds
	m_sounds.LoadSound(section, "snd_draw", "sndShow", false, m_eSoundShow);
	m_sounds.LoadSound(section, "snd_holster", "sndHide", false, m_eSoundHide);

	//Alundaio: LAYERED_SND_SHOOT
	m_sounds.LoadSound(section, "snd_shoot", "sndShot", false, m_eSoundShot);

	if (WeaponSoundExist(section, "snd_shoot_actor", true))
		m_sounds.LoadSound(section, "snd_shoot_actor", "sndShotActor", false, m_eSoundShot);
	//-Alundaio

	if (WeaponSoundExist(section, "snd_shoot_last", true))
		m_sounds.LoadSound(section, "snd_shoot_last", "sndShotLast", false, m_eSoundShot);

	m_sSndShotCurrent = "sndShot";

	m_sounds.LoadSound(section, "snd_empty", "sndEmptyClick", false, m_eSoundEmptyClick);
	m_sounds.LoadSound(section, "snd_reload", "sndReload", true, m_eSoundReload);
	m_sounds.LoadSound(section, "snd_reflect", "sndReflect", true, m_eSoundReflect);

	if (WeaponSoundExist(section, "snd_change_zoom", true))
		m_sounds.LoadSound(section, "snd_change_zoom", "sndChangeZoom", m_eSoundEmptyClick);

	// ����� �� ������ ���������
	if (WeaponSoundExist(section, "snd_close", true))
		m_sounds.LoadSound(section, "snd_close", "sndClose", false, m_eSoundClose);

	if (WeaponSoundExist(section, "snd_reload_empty", true))
		m_sounds.LoadSound(section, "snd_reload_empty", "sndReloadEmpty", true, m_eSoundReload);
	if (WeaponSoundExist(section, "snd_reload_misfire", true))
		m_sounds.LoadSound(section, "snd_reload_misfire", "sndReloadMisfire", true, m_eSoundReload);
	if (WeaponSoundExist(section, "snd_reload_jammed", true))
		m_sounds.LoadSound(section, "snd_reload_jammed", "sndReloadJammed", true, m_eSoundReload);
	if (WeaponSoundExist(section, "snd_pump_gun", true))
		m_sounds.LoadSound(section, "snd_pump_gun", "sndPumpGun", true, m_eSoundReload);

	if (pSettings->line_exist(section, "bullet_bones"))
	{
		bHasBulletsToHide = true;
		LPCSTR str = pSettings->r_string(section, "bullet_bones");
		for (int i = 0, count = _GetItemCount(str); i < count; ++i)
		{
			string128 bullet_bone_name;
			_GetItem(str, i, bullet_bone_name);
			bullets_bones.push_back(bullet_bone_name);
			bullet_cnt++;
		}

	}

	// TODO: Completely remove magazine with bullets implementation from flamethrower

	// load ammo classes
	m_ammoTypes.clear();
	LPCSTR				S = pSettings->r_string(section, "ammo_class");
	if (S && S[0])
	{
		string128		_ammoItem;
		int				count = _GetItemCount(S);
		for (int it = 0; it < count; ++it)
		{
			_GetItem(S, it, _ammoItem);
			m_ammoTypes.push_back(_ammoItem);
		}
	}

	m_current_fuel_level = pSettings->r_float(section, "current_fuel_level");

	m_charge_speed = pSettings->r_float(section, "charge_speed");

	m_overheating_decrease_speed = pSettings->r_float(section, "overheating_decrease_speed");
	m_overheating_increase_speed_min = pSettings->r_float(section, "overheating_increase_speed_min");
	m_overheating_increase_speed_max = pSettings->r_float(section, "overheating_increase_speed_max");
	m_overheating_reset_level_max = pSettings->r_float(section, "overheating_reset_level_max");

	m_fuel_reduce_speed_charge = pSettings->r_float(section, "fuel_reduce_speed_charge");
	m_fuel_reduce_speed_shoot = pSettings->r_float(section, "fuel_reduce_speed_shoot");

	m_dps = pSettings->r_float(section, "dps");
	m_burn_time = pSettings->r_float(section, "burn_time");

	TraceManager->Load((xr_string(section)+"_trace").c_str());
}

bool CFlamethrower::UseScopeTexture()
{
	return bScopeIsHasTexture;
}

void CFlamethrower::FireStart()
{
	if (!IsMisfire())
	{
		if (IsValid())
		{
			if (!IsWorking() || AllowFireWhileWorking())
			{
				if (GetState() == eReload)
					return;
				if (GetState() == eShowing)
					return;
				if (GetState() == eHiding)
					return;
				if (GetState() == eMisfire)
					return;
				if (GetState() == eUnMisfire)
					return;

				inherited::FireStart();

				if (iAmmoElapsed == 0)
					OnMagazineEmpty();
				else {
					R_ASSERT(H_Parent());
					SwitchState(eFire);
				}
			}
		}
		else
		{
			if (eReload != GetState())
				OnMagazineEmpty();
		}
	}
	else
	{
		//misfire
		CGameObject* object = smart_cast<CGameObject*>(H_Parent());
		if (object)
			object->callback(GameObject::eOnWeaponJammed)(object->lua_game_object(), this->lua_game_object());

		if (smart_cast<CActor*>(this->H_Parent()) && (Level().CurrentViewEntity() == H_Parent()))
			CurrentGameUI()->AddCustomStatic("gun_jammed", true);

		OnEmptyClick();
	}
}

void CFlamethrower::FireEnd()
{
	inherited::FireEnd();

	if (m_bAutoreloadEnabled)
	{
		CActor* actor = smart_cast<CActor*>(H_Parent());

		if (Actor()->mstate_real & (mcSprint) && !GameConstants::GetReloadIfSprint())
			return;

		if (m_pInventory && !iAmmoElapsed && actor && GetState() != eReload)
			Reload();
	}
}

void CFlamethrower::Reload()
{
	inherited::Reload();
	TryReload();
}

void CFlamethrower::OnMotionMark(u32 state, const motion_marks& M)
{
	inherited::OnMotionMark(state, M);
	if (state == eReload)
	{
		u8 ammo_type = m_ammoType;
		int ae = CheckAmmoBeforeReload(ammo_type);

		if (ammo_type == m_ammoType)
		{
			Msg("Ammo elapsed: %d", iAmmoElapsed);
			ae += iAmmoElapsed;
		}

		last_hide_bullet = ae >= bullet_cnt ? bullet_cnt : bullet_cnt - ae - 1;

		Msg("Next reload: count %d with type %d", ae, ammo_type);

		HUD_VisualBulletUpdate();
	}
}

bool CFlamethrower::TryReload()
{
	if (m_pInventory)
	{
		if (IsGameTypeSingle() && ParentIsActor())
		{
			int	AC = GetSuitableAmmoTotal();
			Actor()->callback(GameObject::eWeaponNoAmmoAvailable)(lua_game_object(), AC);
		}

		AmmoCanister = smart_cast<CFlameCanister*>(m_pInventory->GetAny(m_ammoTypes[m_ammoType].c_str()));

		if (IsMisfire() && m_current_fuel_level)
		{
			SetPending(TRUE);
			SwitchState(eUnMisfire);
			return				true;
		}

		if (AmmoCanister || unlimited_ammo())
		{
			SetPending(TRUE);
			SwitchState(eReload);
			return				true;
		}
		else for (u8 i = 0; i < static_cast<u8>(m_ammoTypes.size()); ++i)
		{
			for (u32 i = 0; i < m_ammoTypes.size(); ++i)
			{
				AmmoCanister = smart_cast<CFlameCanister*>(m_pInventory->GetAny(*m_ammoTypes[i]));
				if (AmmoCanister)
				{
					m_set_next_ammoType_on_reload = i;
					SetPending(TRUE);
					SwitchState(eReload);
					return				true;
				}
			}
		}

	}

	if (GetState() != eIdle)
		SwitchState(eIdle);

	return false;
}

bool CFlamethrower::IsAmmoAvailable()
{
	if (smart_cast<CFlameCanister*>(m_pInventory->GetAny(m_ammoTypes[m_ammoType].c_str())))
		return true;
	else
	{
		for (u32 i = 0; i < m_ammoTypes.size(); ++i)
		{
			if (smart_cast<CFlameCanister*>(m_pInventory->GetAny(m_ammoTypes[i].c_str())))
				return true;
		}
	}
	return false;
}

void CFlamethrower::OnMagazineEmpty()
{
	if (IsGameTypeSingle() && ParentIsActor())
	{
		int AC = GetSuitableAmmoTotal();
		Actor()->callback(GameObject::eOnWeaponMagazineEmpty)(lua_game_object(), AC);
	}

	if (GetState() == eIdle)
	{
		OnEmptyClick();
		return;
	}

	if (GetNextState() != eMagEmpty && GetNextState() != eReload)
	{
		SwitchState(eMagEmpty);
	}

	inherited::OnMagazineEmpty();
}

void CFlamethrower::UnloadMagazine(bool spawn_ammo)
{
	last_hide_bullet = -1;
	HUD_VisualBulletUpdate();

	if (IsGameTypeSingle() && ParentIsActor())
	{
		int AC = GetSuitableAmmoTotal();
		Actor()->callback(GameObject::eOnWeaponMagazineEmpty)(lua_game_object(), AC);
	}

	if (!spawn_ammo)
		return;

	if (!unlimited_ammo()) {
		SpawnFuelCanister(m_current_fuel_level, m_fuel_section_name.c_str());
	}

	SwitchState(eIdle);
}

int CFlamethrower::CheckAmmoBeforeReload(u8& v_ammoType)
{
	if (m_set_next_ammoType_on_reload != undefined_ammo_type)
		v_ammoType = m_set_next_ammoType_on_reload;

	Msg("Ammo type in next reload : %d", m_set_next_ammoType_on_reload);

	if (m_ammoTypes.size() <= v_ammoType)
	{
		Msg("Ammo type is wrong : %d", v_ammoType);
		return 0;
	}

	LPCSTR tmp_sect_name = m_ammoTypes[v_ammoType].c_str();

	if (!tmp_sect_name)
	{
		Msg("Sect name is wrong");
		return 0;
	}

	CFlameCanister* ammo = smart_cast<CFlameCanister*>(m_pInventory->GetAny(tmp_sect_name));

	if (!ammo && !m_bLockType)
	{
		for (u8 i = 0; i < static_cast<u8>(m_ammoTypes.size()); ++i)
		{
			//��������� ������� ���� ���������� �����
			ammo = smart_cast<CFlameCanister*>(m_pInventory->GetAny(m_ammoTypes[i].c_str()));
			if (ammo)
			{
				v_ammoType = i;
				break;
			}
		}
	}

	Msg("Ammo type %d", v_ammoType);

	return GetAmmoCount(v_ammoType);

}

void CFlamethrower::ReloadMagazine()
{
	m_BriefInfo_CalcFrame = 0;

	//��������� ������ ��� �����������
	if (IsMisfire())	bMisfire = false;

	if (!m_bLockType)
	{
		AmmoCanister = nullptr;
	}

	if (!m_pInventory) return;

	if (m_set_next_ammoType_on_reload != undefined_ammo_type)
	{
		m_ammoType = m_set_next_ammoType_on_reload;
		m_set_next_ammoType_on_reload = undefined_ammo_type;
	}

	if (!unlimited_ammo())
	{
		if (m_ammoTypes.size() <= m_ammoType)
			return;

		LPCSTR tmp_sect_name = m_ammoTypes[m_ammoType].c_str();

		if (!tmp_sect_name)
			return;

		xr_vector<PIItem> canisters;
		m_pInventory->GetAll(tmp_sect_name, canisters);

		//���������� ����� � ��������� ������� �������� ���� 
		//AmmoCanister = smart_cast<CFlameCanister*>(m_pInventory->GetAny(tmp_sect_name));
		for(int i = 0; i < canisters.size(); ++i)
		{
			AmmoCanister = smart_cast<CFlameCanister*>(canisters[i]);
			float Cond = AmmoCanister->GetCondition();
			Cond -= (1.0f - m_current_fuel_level);
			if (Cond <= 0.0)
			{
				m_current_fuel_level += AmmoCanister->GetCondition();
				AmmoCanister->SetCondition(0.0f);
				AmmoCanister->SetDropManual(TRUE);
			}
			else
			{
				AmmoCanister->SetCondition(Cond);
				m_current_fuel_level = 1.0f;
			}
		}
		/*while(AmmoCanister && m_current_fuel_level < 1.0f)
		{
			float Cond = AmmoCanister->GetCondition();
			Cond -= (1.0f - m_current_fuel_level);
			if(Cond <= 0.0)
			{
				m_current_fuel_level += AmmoCanister->GetCondition();
				AmmoCanister->SetCondition(0.0f);
				AmmoCanister->SetDropManual(TRUE);
			} else
			{
				AmmoCanister->SetCondition(Cond);
				m_current_fuel_level = 1.0f;
			}
			AmmoCanister = smart_cast<CFlameCanister*>(m_pInventory->GetAny(tmp_sect_name));
		}*/
	}
}

void CFlamethrower::OnStateSwitch(u32 S)
{
	HUD_VisualBulletUpdate();

	inherited::OnStateSwitch(S);
	CInventoryOwner* owner = smart_cast<CInventoryOwner*>(this->H_Parent());
	switch (S)
	{
	case eIdle:
		switch2_Idle();
		break;
	case eFire:
		switch2_Fire();
		break;
	case eUnMisfire:
		if (owner)
			m_sounds_enabled = owner->CanPlayShHdRldSounds();
		switch2_Unmis();
		break;
	case eMisfire:
		if (smart_cast<CActor*>(this->H_Parent()) && (Level().CurrentViewEntity() == H_Parent()))
			CurrentGameUI()->AddCustomStatic("gun_jammed", true);
		break;
	case eMagEmpty:
		switch2_Empty();
		break;
	case eReload:
		if (owner)
			m_sounds_enabled = owner->CanPlayShHdRldSounds();
		switch2_Reload();
		break;
	case eShowing:
		if (owner)
			m_sounds_enabled = owner->CanPlayShHdRldSounds();
		switch2_Showing();
		break;
	case eHiding:
		if (owner)
			m_sounds_enabled = owner->CanPlayShHdRldSounds();
		switch2_Hiding();
		break;
	case eHidden:
		switch2_Hidden();
		break;
	}
}


void CFlamethrower::UpdateCL()
{
	inherited::UpdateCL();
	float dt = Device.fTimeDelta;

	//Msg("Update flamethrower: dt = [%f]", dt);

	//����� ���������� ������ ��������� ������
	//������ ������� �� ������
	if (GetNextState() == GetState())
	{
		switch (GetState())
		{
		case eShowing:
		case eHiding:
		case eReload:
		case eSprintStart:
		case eSprintEnd:
		case eIdle:
		{
			if (m_keep_charge) {
				state_FireCharge(dt);
				//Msg("State: idle, keep charge. Charge - [%f], fuel - [%f], overheat - [%f]", m_current_charge, m_current_fuel_level, m_overheating_state);
			}
			else {
				fShotTimeCounter -= dt;
				clamp(fShotTimeCounter, 0.0f, flt_max);
				state_Idle(dt);
				//Msg("State: idle, uncharge. Charge - [%f], fuel - [%f], overheat - [%f]", m_current_charge, m_current_fuel_level, m_overheating_state);
			}
			break;
		}
		case eFire:
		{
			if(m_current_charge < 1.0f)
			{
				state_FireCharge(dt);
				//Msg("State: fire, charge. Charge - [%f], fuel - [%f], overheat - [%f]", m_current_charge, m_current_fuel_level, m_overheating_state);
			}
			else {
				state_Fire(dt);
				//Msg("State: fire, shoot. Charge - [%f], fuel - [%f], overheat - [%f]", m_current_charge, m_current_fuel_level, m_overheating_state);
			}
			break;
		}
		case eMisfire:		state_Misfire(dt);	break;
		case eMagEmpty:		state_MagEmpty(dt);	break;
		case eHidden:		break;
		}
	}

	UpdateSounds();

	TraceManager->UpdatePoints(dt);
	TraceManager->UpdateJoins(dt);
	TraceManager->UpdateOverlaps(dt);
}

void CFlamethrower::UpdateSounds()
{
	if (Device.dwFrame == dwUpdateSounds_Frame)
		return;

	dwUpdateSounds_Frame = Device.dwFrame;

	Fvector P = get_LastFP();
	m_sounds.SetPosition("sndShow", P);
	m_sounds.SetPosition("sndHide", P);
	if (psWpnAnimsFlag.test(ANM_HIDE_EMPTY) && WeaponSoundExist(m_section_id.c_str(), "snd_close"))
		m_sounds.SetPosition("sndClose", P);
	if (WeaponSoundExist(m_section_id.c_str(), "snd_change_zoom"))
		m_sounds.SetPosition("sndChangeZoom", P);

	//. nah	m_sounds.SetPosition("sndShot", P);
	m_sounds.SetPosition("sndReload", P);
	//. nah	m_sounds.SetPosition("sndEmptyClick", P);
}

void CFlamethrower::state_FireCharge(float dt)
{
	if(!IsWorking()&&!m_keep_charge)
	{
		StopShooting();
		return;
	}
	if (m_current_fuel_level > 0) {
		m_current_charge += m_charge_speed * dt;
		clamp(m_current_charge, 0.0f, 1.0f);
		m_current_fuel_level -= m_fuel_reduce_speed_charge * dt;
		clamp(m_current_fuel_level, 0.0f, 1.0f);
	} else
	{
		OnMagazineEmpty();
		StopShooting();
	}
}

void CFlamethrower::state_Fire(float dt)
{
	if (m_current_fuel_level > 0)
	{
		VERIFY(fOneShotTime > 0.f);

		Fvector					p1, d;
		p1.set(get_LastFP());
		d.set(get_LastFD());

		if (!H_Parent()) return;
		if (smart_cast<CMPPlayersBag*>(H_Parent()) != nullptr)
		{
			Msg("! WARNING: state_Fire of object [%d][%s] while parent is CMPPlayerBag...", ID(), cNameSect().c_str());
			return;
		}

		CInventoryOwner* io = smart_cast<CInventoryOwner*>(H_Parent());
		if (nullptr == io->inventory().ActiveItem())
		{
			Log("current_state", GetState());
			Log("next_state", GetNextState());
			Log("item_sect", cNameSect().c_str());
			Log("H_Parent", H_Parent()->cNameSect().c_str());
		}

		CEntity* E = smart_cast<CEntity*>(H_Parent());
		E->g_fireParams(this, p1, d);

		if (!E->g_stateFire()) {
			StopShooting();
		}

		m_vStartPos = p1;
		m_vStartDir = d;

		//while (m_current_fuel_level && IsWorking()) {
			if (CheckForMisfire())
			{
				StopShooting();
				return;
			}
			OnShot();
			m_current_fuel_level -= (m_fuel_reduce_speed_charge + m_fuel_reduce_speed_shoot) * dt;
			clamp(m_current_fuel_level, 0.0f, 1.0f);
		//}

		/*VERIFY(!m_magazine.empty());

		while (!m_magazine.empty() &&
			fShotTimeCounter < 0 &&
			(IsWorking() || m_bFireSingleShot)
			)
		{
			if (CheckForMisfire())
			{
				StopShooting();
				return;
			}

			m_bFireSingleShot = false;

			fShotTimeCounter += fOneShotTime;

			OnShot();

			FireTrace(p1, d);
		}*/

		UpdateSounds();
	}

	//if (fShotTimeCounter < 0)
	//{
		/*
				if(bDebug && H_Parent() && (H_Parent()->ID() != Actor()->ID()))
				{
					Msg("stop shooting w=[%s] magsize=[%d] sshot=[%s] qsize=[%d] shotnum=[%d]",
							IsWorking()?"true":"false",
							m_magazine.size(),
							m_bFireSingleShot?"true":"false",
							m_iQueueSize,
							m_iShotNum);
				}
		*/
		if (!m_current_fuel_level) {
			OnMagazineEmpty();
			StopShooting();
			return;
		}

		if(!IsWorking()){
			StopShooting();
		}
	/*}
	else
	{
		fShotTimeCounter -= dt;
	}*/

	//if (m_fFactor > 0)
		//StopShooting();
}

void CFlamethrower::state_Idle(float dt)
{
	m_current_charge = m_current_charge - m_charge_speed * dt;
	clamp(m_current_charge, 0.0f, 1.0f);
}

void CFlamethrower::state_Misfire(float dt)
{
	OnEmptyClick();
	SwitchState(eIdle);

	bMisfire = true;

	UpdateSounds();
}

void CFlamethrower::state_MagEmpty(float dt)
{
}

void CFlamethrower::SetDefaults()
{
	CWeapon::SetDefaults();
}


void CFlamethrower::OnShot()
{
	// ���� ����� ����� - ������������� ���
	if (ParentIsActor() && GameConstants::GetStopActorIfShoot())
		Actor()->set_state_wishful(Actor()->get_state_wishful() & (~mcSprint));

	// Camera	
	AddShotEffector();

	// Animation
	PlayAnimShoot();

	HUD_VisualBulletUpdate();

	// Shell Drop
	Fvector vel;
	PHGetLinearVell(vel);
	OnShellDrop(get_LastSP(), vel);

	// ����� �� ������
	StartFlameParticles();

	//��� �� ������
	ForceUpdateFireParticles();
	//StartSmokeParticles(get_LastFP(), vel);

	// ��������� ���� ����� ��������, ���� �� ����� �������� �� ����� ������ ��� ������ � ��� ����
	if (m_sounds.FindSoundItem("sndPumpGun", false))
		PlaySound("sndPumpGun", get_LastFP());

	if (ParentIsActor())
	{
		luabind::functor<void> funct;
		if (ai().script_engine().functor("mfs_functions.on_actor_shoot", funct))
			funct();

		string128 sndName;
		strconcat(sizeof(sndName), sndName, m_sSndShotCurrent.c_str(), "Actor");
		if (m_sounds.FindSoundItem(sndName, false))
		{
			m_sounds.PlaySound(sndName, get_LastFP(), H_Root(), !!GetHUDmode(), false, static_cast<u8>(-1));
			return;
		}
	}

	string128 sndName;
	strconcat(sizeof(sndName), sndName, m_sSndShotCurrent.c_str(), (iAmmoElapsed == 1) ? "Last" : "");

	if (m_sounds.FindSoundItem(sndName, false)) {
		m_sounds.PlaySound(sndName, get_LastFP(), H_Root(), !!GetHUDmode(), false, static_cast<u8>(-1));
	}
	else {
		m_sounds.PlaySound(m_sSndShotCurrent.c_str(), get_LastFP(), H_Root(), !!GetHUDmode(), false, static_cast<u8>(-1));
	}

	TraceManager->LaunchTrace(m_vStartPos, m_vStartDir);

	// ��� ��������
	if (IsSilencerAttached() == false)
	{
		bool bIndoor = false;
		if (H_Parent() != nullptr)
		{
			bIndoor = H_Parent()->renderable_ROS()->get_luminocity_hemi() < WEAPON_INDOOR_HEMI_FACTOR;
		}

		if (bIndoor && m_sounds.FindSoundItem("sndReflect", false))
		{
			if (IsHudModeNow())
			{
				HUD_SOUND_ITEM::SetHudSndGlobalVolumeFactor(WEAPON_SND_REFLECTION_HUD_FACTOR);
			}
			PlaySound("sndReflect", get_LastFP());
			HUD_SOUND_ITEM::SetHudSndGlobalVolumeFactor(1.0f);
		}
	}

	CGameObject* object = smart_cast<CGameObject*>(H_Parent());
	if (object)
		object->callback(GameObject::eOnWeaponFired)(object->lua_game_object(), this->lua_game_object(), iAmmoElapsed);
}

void CFlamethrower::StopShooting()
{
	inherited::StopShooting();

	TraceManager->OnShootingEnd();
}


void CFlamethrower::OnEmptyClick()
{
	PlaySound("sndEmptyClick", get_LastFP());
}

void CFlamethrower::OnAnimationEnd(u32 state)
{
	switch (state)
	{
	case eReload:
	{
		//CheckMagazine(); // �������� �� ��������� �� Lost Alpha: New Project
		// ������: rafa & Kondr48

		CCartridge FirstBulletInGun;

		bool bNeedputBullet = iAmmoElapsed > 0;

		if (m_bNeedBulletInGun && bNeedputBullet)
		{
			FirstBulletInGun = m_magazine.back();
			m_magazine.pop_back();
			iAmmoElapsed--;
		}

		ReloadMagazine();

		if (m_bNeedBulletInGun && bNeedputBullet)
		{
			m_magazine.push_back(FirstBulletInGun);
			iAmmoElapsed++;
		}

		SwitchState(eIdle);

	}break;// End of reload animation
	case eHiding:	SwitchState(eHidden);   break;	// End of Hide
	case eShowing:	SwitchState(eIdle);		break;	// End of Show
	case eIdle:		switch2_Idle();			break;  // Keep showing idle
	case eUnMisfire:
	{
		bMisfire = false;
		m_magazine.pop_back();
		iAmmoElapsed--;
		SwitchState(eIdle);
		break;
	} // End of UnMisfire animation
	}
	inherited::OnAnimationEnd(state);
}

void CFlamethrower::switch2_Idle()
{
	if (m_fOldBulletSpeed != 0.f)
		SetBulletSpeed(m_fOldBulletSpeed);

	SetPending(FALSE);
	PlayAnimIdle();
}

#ifdef DEBUG
#include "ai\stalker\ai_stalker.h"
#include "object_handler_planner.h"
#endif
void CFlamethrower::switch2_Fire()
{
	CInventoryOwner* io = smart_cast<CInventoryOwner*>(H_Parent());
	CInventoryItem* ii = smart_cast<CInventoryItem*>(this);
#ifdef DEBUG
	if (!io)
		return;
	//VERIFY2					(io,make_string("no inventory owner, item %s",*cName()));

	if (ii != io->inventory().ActiveItem())
		Msg("! not an active item, item %s, owner %s, active item %s", *cName(), *H_Parent()->cName(), io->inventory().ActiveItem() ? *io->inventory().ActiveItem()->object().cName() : "no_active_item");

	if (!(io && (ii == io->inventory().ActiveItem())))
	{
		CAI_Stalker* stalker = smart_cast<CAI_Stalker*>(H_Parent());
		if (stalker) {
			stalker->planner().show();
			stalker->planner().show_current_world_state();
			stalker->planner().show_target_world_state();
		}
	}
#else
	if (!io)
		return;
#endif // DEBUG

	//
	//	VERIFY2(
	//		io && (ii == io->inventory().ActiveItem()),
	//		make_string(
	//			"item[%s], parent[%s]",
	//			*cName(),
	//			H_Parent() ? *H_Parent()->cName() : "no_parent"
	//		)
	//	);

	m_bFireSingleShot = true;

	if ((OnClient() || Level().IsDemoPlay()) && !IsWorking())
		FireStart();

}

void CFlamethrower::switch2_Empty()
{
	OnZoomOut();

	if (m_bAutoreloadEnabled)
	{
		if (!TryReload())
		{
			OnEmptyClick();
		}
		else
		{
			inherited::FireEnd();
		}
	}
	else
	{
		OnEmptyClick();
	}
}
void CFlamethrower::PlayReloadSound()
{
	if (m_sounds_enabled)
	{
		if (iAmmoElapsed == 0)
			if (m_sounds.FindSoundItem("sndReloadEmpty", false) && psWpnAnimsFlag.test(ANM_RELOAD_EMPTY))
				PlaySound("sndReloadEmpty", get_LastFP());
			else
				PlaySound("sndReload", get_LastFP());
		else
			PlaySound("sndReload", get_LastFP());
	}
}

void CFlamethrower::switch2_Reload()
{
	CWeapon::FireEnd();

	PlayReloadSound();
	PlayAnimReload();
	SetPending(TRUE);
}
void CFlamethrower::switch2_Hiding()
{
	OnZoomOut();
	CWeapon::FireEnd();

	if (m_sounds_enabled)
	{
		if (iAmmoElapsed == 0 && psWpnAnimsFlag.test(ANM_HIDE_EMPTY) && WeaponSoundExist(m_section_id.c_str(), "snd_close"))
			PlaySound("sndClose", get_LastFP());
		else
			PlaySound("sndHide", get_LastFP());
	}

	PlayAnimHide();
	SetPending(TRUE);
}

void CFlamethrower::switch2_Unmis()
{
	VERIFY(GetState() == eUnMisfire);

	if (m_sounds_enabled)
	{
		if (m_sounds.FindSoundItem("sndReloadMisfire", false) && isHUDAnimationExist("anm_reload_misfire"))
			PlaySound("sndReloadMisfire", get_LastFP());
		else if (m_sounds.FindSoundItem("sndReloadJammed", false) && isHUDAnimationExist("anm_reload_jammed"))
			PlaySound("sndReloadJammed", get_LastFP());
		else
			PlayReloadSound();
	}

	if (isHUDAnimationExist("anm_reload_misfire"))
		PlayHUDMotionIfExists({ "anm_reload_misfire", "anm_reload" }, true, GetState());
	else if (isHUDAnimationExist("anm_reload_jammed"))
		PlayHUDMotionIfExists({ "anm_reload_jammed", "anm_reload" }, true, GetState());
	else
		PlayAnimReload();
}

void CFlamethrower::switch2_Hidden()
{
	CWeapon::FireEnd();

	StopCurrentAnimWithoutCallback();

	signal_HideComplete();
	RemoveShotEffector();

	if (pSettings->line_exist(item_sect, "hud_fov"))
		m_nearwall_last_hud_fov = m_base_fov;
	else
		m_nearwall_last_hud_fov = psHUD_FOV_def;
}
void CFlamethrower::switch2_Showing()
{
	if (m_sounds_enabled)
		PlaySound("sndShow", get_LastFP());

	SetPending(TRUE);
	PlayAnimShow();
}

#include "CustomDetector.h"

bool CFlamethrower::Action(u16 cmd, u32 flags)
{
	if (inherited::Action(cmd, flags)) return true;

	//���� ������ ���-�� ������, �� ������ �� ������
	if (IsPending()) return false;

	switch (cmd)
	{
	case kWPN_RELOAD:
	{
		if (Actor()->mstate_real & (mcSprint) && !GameConstants::GetReloadIfSprint()) {
			break;
		}
		if (flags & CMD_START) {
			if (m_current_fuel_level < 1.0 || IsMisfire())
			{
				if (GetState() == eUnMisfire) // Rietmon: ��������� �����������, ���� ������ ����� �������������� �������
					return false;

				PIItem Det = Actor()->inventory().ItemFromSlot(DETECTOR_SLOT);
				if (!Det)
					Reload(); // Rietmon: ���� � ����� ���� ���������, �� �� �� ����� ���� �������

				if (Det)
				{
					CCustomDetector* pDet = smart_cast<CCustomDetector*>(Det);
					if (!pDet->IsWorking())
						Reload();
				}
			}
		}
		return true;
	}
	case kWPN_ZOOM: {
		if (flags & CMD_START) {
			if (GetState() == eIdle) {
				PlayAnimIdle();
			}

			//Alundaio: callback not sure why vs2013 gives error, it's fine
			CGameObject* object = smart_cast<CGameObject*>(H_Parent());

			if (object) {
				object->callback(GameObject::eOnWeaponZoomIn)(object->lua_game_object(), this->lua_game_object());
			}
			//-Alundaio

			m_keep_charge = true;
			//return true;
		}
		if (flags & CMD_STOP) {
			if (GetState() == eIdle) {
				PlayAnimIdle();
			}

			//Alundaio
			CGameObject* object = smart_cast<CGameObject*>(H_Parent());
			if (object) {
				object->callback(GameObject::eOnWeaponZoomOut)(object->lua_game_object(), this->lua_game_object());
			}
			//-Alundaio

			m_keep_charge = false;
		}

		return true;
	}
	//return true;
	}
	return false;
}

bool CFlamethrower::CanAttach(PIItem pIItem)
{
	CScope* pScope = smart_cast<CScope*>(pIItem);

	if (pScope &&
		m_eScopeStatus == ALife::eAddonAttachable &&
		(m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonScope) == 0 /*&&
		(m_scopes[cur_scope]->m_sScopeName == pIItem->object().cNameSect())*/)
	{
		SCOPES_VECTOR_IT it = m_scopes.begin();
		for (; it != m_scopes.end(); it++)
		{
			if (bUseAltScope)
			{
				if (*it == pIItem->object().cNameSect())
					return true;
			}
			else
			{
				if (pSettings->r_string((*it), "scope_name") == pIItem->object().cNameSect())
					return true;
			}
		}
		return false;
	}
	return inherited::CanAttach(pIItem);
}

bool CFlamethrower::CanDetach(const char* item_section_name)
{
	if (m_eScopeStatus == ALife::eAddonAttachable &&
		0 != (m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonScope))/* &&
		(m_scopes[cur_scope]->m_sScopeName	== item_section_name))*/
	{
		SCOPES_VECTOR_IT it = m_scopes.begin();
		for (; it != m_scopes.end(); it++)
		{
			if (bUseAltScope)
			{
				if (*it == item_section_name)
					return true;
			}
			else
			{
				if (pSettings->r_string((*it), "scope_name") == item_section_name)
					return true;
			}
		}
		return false;
	}
	return inherited::CanDetach(item_section_name);
}

bool CFlamethrower::Attach(PIItem pIItem, bool b_send_event)
{
	bool result = false;

	CScope* pScope = smart_cast<CScope*>(pIItem);

	if (pScope &&
		m_eScopeStatus == ALife::eAddonAttachable &&
		(m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonScope) == 0 /*&&
		(m_scopes[cur_scope]->m_sScopeName == pIItem->object().cNameSect())*/)
	{
		SCOPES_VECTOR_IT it = m_scopes.begin();
		for (; it != m_scopes.end(); it++)
		{
			if (bUseAltScope)
			{
				if (*it == pIItem->object().cNameSect())
					m_cur_scope = static_cast<u8>(it - m_scopes.begin());
			}
			else
			{
				if (pSettings->r_string((*it), "scope_name") == pIItem->object().cNameSect())
					m_cur_scope = static_cast<u8>(it - m_scopes.begin());
			}
		}
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonScope;
		result = true;
	}

	if (result)
	{
		if (pScope && bUseAltScope)
		{
			bNVsecondVPstatus = !!pSettings->line_exist(pIItem->object().cNameSect(), "scope_nightvision");
		}

		if (b_send_event && OnServer())
		{
			//���������� �������������� ���� �� ���������
//.			pIItem->Drop					();
			pIItem->object().DestroyObject();
		};

		UpdateAltScope();
		UpdateAddonsVisibility();
		InitAddons();

		return true;
	}
	else
		return inherited::Attach(pIItem, b_send_event);
}

bool CFlamethrower::DetachScope(const char* item_section_name, bool b_spawn_item)
{
	bool detached = false;
	SCOPES_VECTOR_IT it = m_scopes.begin();
	shared_str iter_scope_name = "none";
	for (; it != m_scopes.end(); it++)
	{
		if (bUseAltScope)
		{
			iter_scope_name = (*it);
		}
		else
		{
			iter_scope_name = pSettings->r_string((*it), "scope_name");
		}
		if (!xr_strcmp(iter_scope_name, item_section_name))
		{
			m_cur_scope = NULL;
			m_cur_scope_bone = nullptr;
			detached = true;
		}
	}
	return detached;
}

bool CFlamethrower::Detach(const char* item_section_name, bool b_spawn_item)
{
	if (m_eScopeStatus == ALife::eAddonAttachable &&
		DetachScope(item_section_name, b_spawn_item))
	{
		if ((m_flagsAddOnState & CSE_ALifeItemWeapon::eWeaponAddonScope) == 0)
		{
			Msg("ERROR: scope addon already detached.");
			return true;
		}
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonScope;

		UpdateAltScope();
		UpdateAddonsVisibility();
		InitAddons();

		return CInventoryItemObject::Detach(item_section_name, b_spawn_item);
	}
	return inherited::Detach(item_section_name, b_spawn_item);;
}

void CFlamethrower::InitAddons()
{
	m_zoom_params.m_fIronSightZoomFactor = READ_IF_EXISTS(pSettings, r_float, cNameSect(), "ironsight_zoom_factor", 50.0f);

	SetAnimFlag(ANM_SHOT_AIM, "anm_shots_when_aim");
	SetAnimFlag(ANM_SHOT_AIM_GL, "anm_shots_w_gl_when_aim");

	m_weapon_attaches.clear();

	if (IsScopeAttached())
	{
		if (m_eScopeStatus == ALife::eAddonAttachable)
		{
			LoadCurrentScopeParams(GetScopeName().c_str());

			if (pSettings->line_exist(m_scopes[m_cur_scope], "bones"))
			{
				pcstr ScopeBone = pSettings->r_string(m_scopes[m_cur_scope], "bones");
				m_cur_scope_bone = ScopeBone;
			}

			if (m_sScopeAttachSection.size() && pSettings->line_exist(m_sScopeAttachSection, "attach_hud_visual"))
				WeaponAttach().CreateAttach(m_sScopeAttachSection, m_weapon_attaches);
		}
		else if (m_eScopeStatus == ALife::eAddonPermanent)
		{
			if (m_sScopeAttachSection.size() && pSettings->line_exist(m_sScopeAttachSection, "attach_hud_visual"))
				WeaponAttach().CreateAttach(m_sScopeAttachSection, m_weapon_attaches);
		}
	}
	else
	{
		if (m_sScopeAttachSection.size() && pSettings->line_exist(m_sScopeAttachSection, "attach_hud_visual"))
			WeaponAttach().RemoveAttach(m_sScopeAttachSection, m_weapon_attaches);

		if (m_UIScope)
			xr_delete(m_UIScope);

		if (bIsSecondVPZoomPresent())
			m_zoom_params.m_fSecondVPFovFactor = 0.0f;

		if (IsZoomEnabled())
			m_zoom_params.m_fIronSightZoomFactor = pSettings->r_float(cNameSect(), "scope_zoom_factor");
	}

	{
		//m_sFlameParticlesCurrent = m_sFlameParticles;
		m_sFlameParticlesCurrent = "";
		m_sSmokeParticlesCurrent = m_sSmokeParticles;
		m_sSndShotCurrent = "sndShot";

		//��������� �� ��������
		LoadLights(*cNameSect(), "");
	}

	inherited::InitAddons();
}

void CFlamethrower::PlayAnimShow()
{
	VERIFY(GetState() == eShowing);

	if (iAmmoElapsed >= 1)
		m_opened = false;
	else
		m_opened = true;

	HUD_VisualBulletUpdate();

	if (iAmmoElapsed == 0 && psWpnAnimsFlag.test(ANM_SHOW_EMPTY))
		PlayHUDMotion("anm_show_empty", FALSE, this, GetState());
	else if (IsMisfire() && isHUDAnimationExist("anm_show_jammed"))
		PlayHUDMotion("anm_show_jammed", false, this, GetState());
	else
		PlayHUDMotion("anm_show", FALSE, this, GetState());
}

void CFlamethrower::PlayAnimHide()
{
	VERIFY(GetState() == eHiding);

	if (iAmmoElapsed == 0 && psWpnAnimsFlag.test(ANM_HIDE_EMPTY))
		PlayHUDMotion("anm_hide_empty", TRUE, this, GetState());
	else if (IsMisfire() && isHUDAnimationExist("anm_hide_jammed"))
		PlayHUDMotion("anm_hide_jammed", true, this, GetState());
	else
		PlayHUDMotion("anm_hide", TRUE, this, GetState());
}

void CFlamethrower::PlayAnimBore()
{
	inherited::PlayAnimBore();
}

void CFlamethrower::PlayAnimIdleSprint()
{
	if (iAmmoElapsed == 0 && psWpnAnimsFlag.test(ANM_SPRINT_EMPTY))
		PlayHUDMotion("anm_idle_sprint_empty", TRUE, nullptr, GetState());
	else if (IsMisfire() && isHUDAnimationExist("anm_idle_sprint_jammed"))
		PlayHUDMotion("anm_idle_sprint_jammed", true, nullptr, GetState());
	else
		inherited::PlayAnimIdleSprint();
}

void CFlamethrower::PlayAnimIdleMoving()
{
	if (iAmmoElapsed == 0 && psWpnAnimsFlag.test(ANM_MOVING_EMPTY))
		PlayHUDMotion("anm_idle_moving_empty", TRUE, nullptr, GetState());
	else if (IsMisfire() && isHUDAnimationExist("anm_idle_moving_jammed"))
		PlayHUDMotion("anm_idle_moving_jammed", true, nullptr, GetState());
	else
		inherited::PlayAnimIdleMoving();
}

void CFlamethrower::PlayAnimReload()
{
	VERIFY(GetState() == eReload);

	if (iAmmoElapsed == 0)
		PlayHUDMotionIfExists({ "anm_reload_empty", "anm_reload" }, true, GetState());
	else
		PlayHUDMotion("anm_reload", TRUE, this, GetState());
}

/*void CFlamethrower::PlayAnimAim()
{
	if (IsRotatingToZoom())
	{
		if (isHUDAnimationExist("anm_idle_aim_start"))
		{
			PlayHUDMotionNew("anm_idle_aim_start", true, GetState());
			return;
		}
	}

	if (const char* guns_aim_anm = GetAnimAimName())
	{
		if (isHUDAnimationExist(guns_aim_anm))
		{
			PlayHUDMotionNew(guns_aim_anm, true, GetState());
			return;
		}
		else if (strstr(guns_aim_anm, "_jammed"))
		{
			char new_guns_aim_anm[256];
			strcpy(new_guns_aim_anm, guns_aim_anm);
			new_guns_aim_anm[strlen(guns_aim_anm) - strlen("_jammed")] = '\0';

			if (isHUDAnimationExist(new_guns_aim_anm))
			{
				PlayHUDMotionNew(new_guns_aim_anm, true, GetState());
				return;
			}
		}
		else if (strstr(guns_aim_anm, "_empty"))
		{
			char new_guns_aim_anm[256];
			strcpy(new_guns_aim_anm, guns_aim_anm);
			new_guns_aim_anm[strlen(guns_aim_anm) - strlen("_empty")] = '\0';

			if (isHUDAnimationExist(new_guns_aim_anm))
			{
				PlayHUDMotionNew(new_guns_aim_anm, true, GetState());
				return;
			}
		}
	}

	if (iAmmoElapsed == 0 && psWpnAnimsFlag.test(ANM_AIM_EMPTY))
		PlayHUDMotion("anm_idle_aim_empty", TRUE, NULL, GetState());
	else if (IsMisfire() && isHUDAnimationExist("anm_idle_aim_jammed"))
		PlayHUDMotion("anm_idle_aim_jammed", true, nullptr, GetState());
	else
		PlayHUDMotion("anm_idle_aim", TRUE, NULL, GetState());
}*/

void CFlamethrower::PlayAnimIdle()
{
	if (GetState() != eIdle)	return;

	if (TryPlayAnimIdle()) return;

	//if (IsZoomed())
		//PlayAnimAim();
	/*else */if (!m_current_fuel_level && psWpnAnimsFlag.test(ANM_IDLE_EMPTY))
		PlayHUDMotion("anm_idle_empty", TRUE, nullptr, GetState());
	else if (IsMisfire() && isHUDAnimationExist("anm_idle_jammed") && !TryPlayAnimIdle())
		PlayHUDMotion("anm_idle_jammed", true, nullptr, GetState());
	else
	{
		/*if (IsRotatingFromZoom())
		{
			if (isHUDAnimationExist("anm_idle_aim_end"))
			{
				PlayHUDMotionNew("anm_idle_aim_end", true, GetState());
				return;
			}
		}*/
		inherited::PlayAnimIdle();
	}
}

void CFlamethrower::PlayAnimShoot()
{
	VERIFY(GetState() == eFire);

	string_path guns_shoot_anm{};
	strconcat(sizeof(guns_shoot_anm), guns_shoot_anm, (isHUDAnimationExist("anm_shoot") ? "anm_shoot" : "anm_shots"), (iAmmoElapsed == 1) ? "_last" : "", (IsZoomed() && !IsRotatingToZoom()) ? (IsScopeAttached() ? "_aim_scope" : "_aim") : "", (IsSilencerAttached() && m_bUseAimSilShotAnim) ? "_sil" : "");

	//HUD_VisualBulletUpdate();

	if (iAmmoElapsed == 1)
		PlayHUDMotionIfExists({ guns_shoot_anm, "anm_shot_l", "anm_shots" }, false, GetState());
	else
		PlayHUDMotionIfExists({ guns_shoot_anm, "anm_shots" }, false, GetState());
}

void CFlamethrower::OnZoomIn()
{
	inherited::OnZoomIn();

	if (GetState() == eIdle)
		PlayAnimIdle();

	//Alundaio: callback not sure why vs2013 gives error, it's fine
	CGameObject* object = smart_cast<CGameObject*>(H_Parent());

	if (object)
		object->callback(GameObject::eOnWeaponZoomIn)(object->lua_game_object(), this->lua_game_object());
	//-Alundaio

	m_keep_charge = true;

	/*CActor* pActor = smart_cast<CActor*>(H_Parent());
	if (pActor)
	{
		CEffectorZoomInertion* S = smart_cast<CEffectorZoomInertion*>	(pActor->Cameras().GetCamEffector(eCEZoom));
		if (!S)
		{
			S = static_cast<CEffectorZoomInertion*>(pActor->Cameras().AddCamEffector(xr_new<CEffectorZoomInertion>()));
			S->Init(this);
		};
		S->SetRndSeed(pActor->GetZoomRndSeed());
		R_ASSERT(S);
	}*/
}
void CFlamethrower::OnZoomOut()
{
	if (!IsZoomed())
		return;

	inherited::OnZoomOut();

	if (GetState() == eIdle)
		PlayAnimIdle();

	//Alundaio
	CGameObject* object = smart_cast<CGameObject*>(H_Parent());
	if (object)
		object->callback(GameObject::eOnWeaponZoomOut)(object->lua_game_object(), this->lua_game_object());
	//-Alundaio

	m_keep_charge = false;

	/*CActor* pActor = smart_cast<CActor*>(H_Parent());

	if (pActor)
		pActor->Cameras().RemoveCamEffector(eCEZoom);*/

}

void	CFlamethrower::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
};

float	CFlamethrower::GetWeaponDeterioration()
{
	// modified by Peacemaker [17.10.08]
	//	if (!m_bHasDifferentFireModes || m_iPrefferedFireMode == -1 || u32(GetCurrentFireMode()) <= u32(m_iPrefferedFireMode)) 
	//		return inherited::GetWeaponDeterioration();
	//	return m_iShotNum*conditionDecreasePerShot;
	return conditionDecreasePerQueueShot;
};

void CFlamethrower::save(NET_Packet& output_packet)
{
	inherited::save(output_packet);
	save_data(m_is_overheated, output_packet);
	save_data(m_overheating_state, output_packet);
	save_data(m_current_charge, output_packet);
	save_data(m_current_fuel_level, output_packet);
	save_data(m_fuel_section_name, output_packet);
	TraceManager->save(output_packet);
}

void CFlamethrower::load(IReader& input_packet)
{
	inherited::load(input_packet);
	load_data(m_is_overheated, input_packet);
	load_data(m_overheating_state, input_packet);
	load_data(m_current_charge, input_packet);
	load_data(m_current_fuel_level, input_packet);
	load_data(m_fuel_section_name, input_packet);
	TraceManager->load(input_packet);
}

void CFlamethrower::Save(CSaveObjectSave* Object) const 
{
	Object->BeginChunk("CFlamethrower");
	{
		inherited::Save(Object);
		Object->GetCurrentChunk()->w_bool(m_is_overheated);
		Object->GetCurrentChunk()->w_float(m_overheating_state);
		Object->GetCurrentChunk()->w_float(m_current_charge);
		Object->GetCurrentChunk()->w_float(m_current_fuel_level);
		Object->GetCurrentChunk()->w_stringZ(m_fuel_section_name);
		TraceManager->Save(Object);
	}
	Object->EndChunk();
}

void CFlamethrower::Load(CSaveObjectLoad* Object) 
{
	Object->FindChunk("CFlamethrower");
	{
		inherited::Load(Object);
		Object->GetCurrentChunk()->r_bool(m_is_overheated);
		Object->GetCurrentChunk()->r_float(m_overheating_state);
		Object->GetCurrentChunk()->r_float(m_current_charge);
		Object->GetCurrentChunk()->r_float(m_current_fuel_level);
		Object->GetCurrentChunk()->r_stringZ(m_fuel_section_name);
		TraceManager->Load(Object);
	}
	Object->EndChunk();
}

void CFlamethrower::SpawnFuelCanister(float Condition, LPCSTR ammoSect, u32 ParentID)
{
	if (OnClient())	return;
	m_bAmmoWasSpawned = true;

	int l_type = 0;
	l_type %= m_ammoTypes.size();

	if (!ammoSect) ammoSect = m_ammoTypes[l_type].c_str();

	++l_type;
	l_type %= m_ammoTypes.size();

	CSE_Abstract* D = F_entity_Create(ammoSect);

	{
		CSE_ALifeItemFuel* l_pA = smart_cast<CSE_ALifeItemFuel*>(D);
		R_ASSERT(l_pA);
		D->s_name = ammoSect;
		D->set_name_replace("");
		//.		D->s_gameid					= u8(GameID());
		D->s_RP = 0xff;
		D->ID = 0xffff;
		if (ParentID == 0xffffffff)
			D->ID_Parent = (u16)H_Parent()->ID();
		else
			D->ID_Parent = static_cast<u16>(ParentID);

		D->ID_Phantom = 0xffff;
		D->s_flags.assign(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime = 0;
		l_pA->m_tNodeID = g_dedicated_server ? static_cast<u32>(-1) : ai_location().level_vertex_id();

		l_pA->m_fCondition = Condition;
	}
	F_entity_Destroy(D);

}

void CFlamethrower::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);
}

void CFlamethrower::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);
}

#include "string_table.h"
bool CFlamethrower::GetBriefInfo(II_BriefInfo& info)
{
	VERIFY(m_pInventory);
	string32	int_str, fire_mode, ammo = "";

	float ae = m_current_fuel_level * 100.0f;
	xr_sprintf(int_str, "%.1f%%", ae);
	info.cur_ammo = int_str;
	info.fire_mode._set("");

	if (bHasBulletsToHide)
	{
		last_hide_bullet = ae >= bullet_cnt ? bullet_cnt : bullet_cnt - ae - 1;

		if (ae == 0) last_hide_bullet = -1;

		//HUD_VisualBulletUpdate();
	}

	info.fire_mode = "";

	/*if (m_pInventory->ModifyFrame() <= m_BriefInfo_CalcFrame)
	{
		return false;
	}*/
	//GetSuitableAmmoTotal();//update m_BriefInfo_CalcFrame
	info.grenade = "";

	u32 at_size = m_ammoTypes.size();
	if (unlimited_ammo() || at_size == 0)
	{
		info.fmj_ammo._set("--");
		info.ap_ammo._set("--");
	}
	else
	{
		//GetSuitableAmmoTotal(); //mp = all type

		// Lex Addon (correct by Suhar_) 28.03.2017		(begin)
		/*int add_ammo_count = 0;

		for (int i = 0; i < at_size; i++)
		{
			if (m_ammoType == i)
			{
				xr_sprintf(int_str, "%d", GetAmmoCount(i));
				info.fmj_ammo = int_str;
			}
			else
			{
				add_ammo_count += GetAmmoCount(i);
			}
		}
		if (at_size > 1)
			xr_sprintf(int_str, "%d", add_ammo_count);
		else
			xr_sprintf(int_str, "%s", "");

		info.ap_ammo = int_str;*/

		xr_sprintf(ammo, "%.1f%%", m_current_charge * 100.0f);
		info.fmj_ammo._set(ammo);
		xr_sprintf(ammo, "%.1f%%", m_overheating_state * 100.0f);
		info.ap_ammo._set(ammo);

		/*if (at_size >= 1 && at_size < 3)
		{
			xr_sprintf(ammo, "%d", (int)(m_current_fuel_level*100.0f));
			info.fmj_ammo._set(ammo);
		}
		if (at_size == 2)
		{
			xr_sprintf(ammo, "%d", GetAmmoCount(1));
			info.ap_ammo._set(ammo);
		}
		if (at_size >= 3)
		{
			xr_sprintf(ammo, "%d", GetAmmoCount(m_ammoType));
			info.fmj_ammo._set(ammo);
			u8 m = 0;
			u64 ap = 0;
			while (m < at_size)
			{
				if (m != m_ammoType)
					ap += GetAmmoCount(m);
				m++;
			}
			xr_sprintf(ammo, "%d", ap);
			info.ap_ammo._set(ammo);
		}*/

		// Lex Addon (correct by Suhar_) 28.07.2017		(end)
	}

	if (ae != 0 && m_magazine.size() != 0)
	{
		LPCSTR ammo_type = m_ammoTypes[m_magazine.back().m_LocalAmmoType].c_str();
		info.name = CStringTable().translate(pSettings->r_string(ammo_type, "inv_name_short"));
		info.icon = ammo_type;
	}
	else
	{
		LPCSTR ammo_type = m_ammoTypes[m_ammoType].c_str();
		info.name = CStringTable().translate(pSettings->r_string(ammo_type, "inv_name_short"));
		info.icon = ammo_type;
	}
	return true;
}

BOOL CFlamethrower::IsMisfire() const
{
	return m_is_overheated;
}

bool CFlamethrower::install_upgrade_impl(LPCSTR section, bool test)
{
	bool result = inherited::install_upgrade_impl(section, test);

	LPCSTR str;

	// sounds (name of the sound, volume (0.0 - 1.0), delay (sec))
	bool result2 = process_if_exists_set(section, "snd_draw", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_draw", "sndShow", false, m_eSoundShow); }
	result |= result2;

	result2 = process_if_exists_set(section, "snd_holster", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_holster", "sndHide", false, m_eSoundHide); }
	result |= result2;

	result2 = process_if_exists_set(section, "snd_shoot", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_shoot", "sndShot", false, m_eSoundShot); }
	result |= result2;

	result2 = process_if_exists_set(section, "snd_empty", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_empty", "sndEmptyClick", false, m_eSoundEmptyClick); }
	result |= result2;

	result2 = process_if_exists_set(section, "snd_reload", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_reload", "sndReload", true, m_eSoundReload); }
	result |= result2;

	result2 = process_if_exists_set(section, "snd_reflect", &CInifile::r_string, str, test);
	if (result2 && !test) { m_sounds.LoadSound(section, "snd_reflect", "sndReflect", false, m_eSoundReflect); }
	result |= result2;

	//snd_shoot1     = weapons\ak74u_shot_1 ??
	//snd_shoot2     = weapons\ak74u_shot_2 ??
	//snd_shoot3     = weapons\ak74u_shot_3 ??

	// fov for zoom mode
	result |= process_if_exists(section, "ironsight_zoom_factor", &CInifile::r_float, m_zoom_params.m_fIronSightZoomFactor, test);

	if (IsScopeAttached())
	{
		//if ( m_eScopeStatus == ALife::eAddonAttachable )
		{
			result |= process_if_exists(section, "scope_zoom_factor", &CInifile::r_float, m_zoom_params.m_fScopeZoomFactor, test);
		}
	}
	else
	{
		if (IsZoomEnabled())
		{
			result |= process_if_exists(section, "scope_zoom_factor", &CInifile::r_float, m_zoom_params.m_fIronSightZoomFactor, test);
		}
	}

	return result;
}

void CFlamethrower::FireBullet(const Fvector& pos,
	const Fvector& shot_dir,
	float fire_disp,
	const CCartridge& cartridge,
	u16 parent_id,
	u16 weapon_id,
	bool send_hit)
{
	inherited::FireBullet(pos, shot_dir, fire_disp, cartridge, parent_id, weapon_id, send_hit);
}

// AVO: for custom added sounds check if sound exists
bool CFlamethrower::WeaponSoundExist(LPCSTR section, LPCSTR sound_name, bool log) const
{
	pcstr str;
	bool sec_exist = process_if_exists_set(section, sound_name, &CInifile::r_string, str, true);
	if (sec_exist)
		return true;
#ifdef DEBUG
	if (log)
		Msg("~ [WARNING] ------ Sound [%s] does not exist in [%s]", sound_name, section);
#endif
	return false;
}

/*void CFlamethrower::CheckMagazine()
{
	if (!ParentIsActor())
	{
		m_bNeedBulletInGun = false;
		return;
	}

	if (psWpnAnimsFlag.test(ANM_RELOAD_EMPTY) && iAmmoElapsed >= 1 && m_bNeedBulletInGun == false)
	{
		m_bNeedBulletInGun = true;
	}
	else if (psWpnAnimsFlag.test(ANM_RELOAD_EMPTY) && iAmmoElapsed == 0 && m_bNeedBulletInGun == true)
	{
		m_bNeedBulletInGun = false;
	}
}*/

#endif