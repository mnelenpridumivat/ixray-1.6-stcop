#pragma once
#include "inventory_item.h"
#include "../xrScripts/script_export_space.h"

class CPhysicItem;
class CEntityAlive;
class CActor;

class CEatableItem : public CInventoryItem {
private:
	typedef CInventoryItem	inherited;

protected:
	CPhysicItem* m_physic_item;

	u8 m_iMaxUses;
	u8 m_iRemainingUses;
	float m_iPortionsMarker;
	BOOL m_bRemoveAfterUse;
	BOOL m_bConsumeChargeOnUse;
	float m_fWeightFull;
	float m_fWeightEmpty;
	bool bUseHUDAnim = false;

public:
	shared_str UseText;
	float m_eat_condition = 1;

public:
	CEatableItem();
	virtual ~CEatableItem();
	virtual	DLL_Pure* _construct();

	virtual CEatableItem* cast_eatable_item() { return this; }

	virtual void			Load(LPCSTR section);
	virtual void			load(IReader& packet);
	virtual void			save(NET_Packet& packet);
	virtual bool			Useful() const;

	virtual BOOL			net_Spawn(CSE_Abstract* DC);

	virtual void			OnH_B_Independent(bool just_before_destroy);
	virtual void			OnH_A_Independent();
	virtual	bool			UseBy(CEntityAlive* npc);
	virtual u32				Cost()	const override;
	virtual float			Weight() const;

	virtual bool				CheckInventoryIconItemSimilarity(CInventoryItem* other) override;

	IC bool Empty() const { return GetRemainingUses() == 0; };
	IC bool CanDelete() const { return m_bRemoveAfterUse == 1; };
	IC bool CanConsumeCharge() const { return m_bConsumeChargeOnUse == 1; };
	void SetRemainingUses(u8 value) { if (value > m_iMaxUses) return; m_iRemainingUses = value; };
	IC u8 GetMaxUses() { return m_iMaxUses; };
	IC u8 GetRemainingUses() const { return m_iRemainingUses; /*return m_iPortionsMarker / m_eat_condition;*/ };
	//IC void SetRemainingUses(u8 value) { m_fCondition = ((float)value / (float)m_iMaxUses); clamp(m_fCondition, 0.f, 1.f); };

	void			UpdateInRuck(CActor* actor);
	void			UpdateUseAnim(CActor* actor);
	void			HideWeapon();
	void			StartAnimation();

	bool			m_bHasAnimation;
	bool			m_bUnlimited;
	bool			m_bActivated;
	bool			m_bItmStartAnim;
	int				m_iAnimHandsCnt;
	int				m_iAnimLength;
	float			m_fEffectorIntensity;
	LPCSTR			anim_sect;
	shared_str		use_cam_effector;
	ref_sound		m_using_sound;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};