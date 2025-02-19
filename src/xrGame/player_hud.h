#pragma once
#include "firedeps.h"

#include "../Include/xrRender/Kinematics.h"
#include "../Include/xrRender/KinematicsAnimated.h"
#include "actor_defs.h"

#include "player_model_controller.h"

class player_hud: public first_person_player_interface
{
public: 
					player_hud			(bool invert = false);
					~player_hud			();
	virtual void			load				(const shared_str& model_name) override;
	virtual void			update				(const Fmatrix& trans) override;
	virtual void			render_hud			() override;	
	virtual void			render_item_ui		() override;
	virtual bool			render_item_ui_query() override;

	virtual u32				anim_play			(u16 part, const MotionID& M, BOOL bMixIn, const CMotionDef*& md, float speed) override;
	virtual bool			check_anim			(const shared_str& anim_name, u16 place_idx) override;

	virtual bool			animator_play			(const shared_str& anim_name, u16 place_idx = u16(-1), u16 part_id = u16(-1), BOOL bMixIn = FALSE, float speed = 1.0f, u8 anm_idx = u8(0), bool impact_on_item = false, bool similar_check = false, PlayCallback Callback = PlayCallback(0), LPVOID CallbackParam = LPVOID(0), BOOL UpdateCallbackType = 0) override;
	virtual void			animator_fx_play		(const shared_str& anim_name, u16 place_idx = u16(-1), u16 part_id = u16(-1), u8 anm_idx = u8(0), float blendAccrue = 1.f, float blendFalloff = 1.f, float Speed = 1.f, float Power = 1.f) override;

	virtual const shared_str& section_name		() const override;

	virtual attachable_hud_item* create_hud_item(const shared_str& sect) override;
	virtual void			RemoveHudItem		(const shared_str& sect) override;
	virtual void			attach_item			(CHudItem* item) override;
	virtual bool			allow_activation	(CHudItem* item) override;
	virtual attachable_hud_item* attached_item	(u16 item_idx) override;
	virtual void			detach_item_idx		(u16 idx) override;
	virtual void			detach_item			(CHudItem* item) override;
	virtual void			detach_all_items	() override;

	virtual void			calc_transform		(u16 attach_slot_idx, const Fmatrix& offset, Fmatrix& result) override;
	virtual void			tune				(Ivector values) override;
	virtual u32				motion_length		(const MotionID& M, const CMotionDef*& md, float speed) override;
	virtual u32				motion_length		(const shared_str& anim_name, const shared_str& hud_name, const CMotionDef*& md) override;
	virtual void			OnMovementChanged	(ACTOR_DEFS::EMoveCommand cmd) override;
	virtual void			RestoreHandBlends(LPCSTR ignored_part) override;

	virtual void			ResetBlockedPartID() override;
	virtual void			SetHandsVisible(bool val) override;
	virtual bool			GetHandsVisible() override;

	virtual IKinematics* GetLegsModel() override;
	virtual bool GetIsLegsShown() override;
	virtual void SetIsLegsShown(bool shown) override;

private:
	void			update_inertion		(Fmatrix& trans);
	void			update_additional	(Fmatrix& trans);
private:
	const Fvector&	attach_rot			() const;
	const Fvector&	attach_pos			() const;

	shared_str							m_sect_name;

	Fmatrix								m_attach_offsetr, m_attach_offsetl;

	Fmatrix								m_transform;
	Fmatrix								m_transformL;

	IKinematicsAnimated*				m_model;
	xr_vector<u16>						m_ancors;
	attachable_hud_item*				m_attached_items[2];
	xr_vector<attachable_hud_item*>		m_pool;

	IKinematics* m_legs_model;
	bool			m_show_legs = true;

	u16									m_blocked_part_idx;
	bool								m_bhands_visible;
	bool								m_binverted;
	int									item_idx_priority;
	void  LeftArmCallback(CBoneInstance* B);
	float CalcMotionSpeed(const shared_str& anim_name);
};

#define g_player_hud CPlayerModelController::GetInstance().GetHud()
//extern player_hud* g_player_hud;