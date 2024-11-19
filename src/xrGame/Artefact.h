#pragma once

#include "hud_item_object.h"
#include "hit_immunity.h"
#include "../xrPhysics/PHUpdateObject.h"
#include "../xrScripts/script_export_space.h"
#include "patrol_path.h"

class SArtefactActivation;
struct SArtefactDetectorsSupport;

class CArtefact :	public CHudItemObject, 
					public CPHUpdateObject 
{
	typedef			CHudItemObject	inherited;
public:
									CArtefact						();
	virtual							~CArtefact						();

	virtual void					Load							(LPCSTR section);
	virtual BOOL					net_Spawn						(CSE_Abstract* DC);
	virtual void					net_Destroy						();

	virtual void					OnH_A_Chield					();
	virtual void					OnH_B_Independent				(bool just_before_destroy);
	virtual void					OnActiveItem					();
	virtual void					OnHiddenItem					();
	
	virtual void					UpdateCL						();
	virtual void					shedule_Update					(u32 dt);	
			void					UpdateWorkload					(u32 dt);

	
	virtual bool					CanTake							() const;

	virtual BOOL					renderable_ShadowGenerate		()		{ return FALSE;	}
	virtual BOOL					renderable_ShadowReceive		()		{ return TRUE;	}
	virtual void					create_physic_shell				();

	virtual CArtefact*				cast_artefact					()		{return this;}

	virtual bool				CheckInventoryIconItemSimilarity(CInventoryItem* other) override;

	virtual float					GetHealthPower() const { return m_fHealthRestoreSpeed; }
	virtual float					GetRadiationPower() const { return m_fRadiationRestoreSpeed; }
	virtual float					GetSatietyPower() const { return m_fSatietyRestoreSpeed; }
	virtual float					GetPowerPower() const { return m_fPowerRestoreSpeed; }
	virtual float					GetBleedingPower() const { return m_fBleedingRestoreSpeed; }
	virtual float					GetJumpPower() const { return m_fJumpSpeed; }
	virtual float					GetWalkPower() const { return m_fWalkAccel; }

	virtual void					SetHealthPower(float value) { m_fHealthRestoreSpeed = value; }
	virtual void					SetRadiationPower(float value) { m_fRadiationRestoreSpeed = value; }
	virtual void					SetSatietyPower(float value) { m_fSatietyRestoreSpeed = value; }
	virtual void					SetPowerPower(float value) { m_fPowerRestoreSpeed = value; }
	virtual void					SetBleedingPower(float value) { m_fBleedingRestoreSpeed = value; }
	virtual void					SetJumpPower(float value) { m_fJumpSpeed = value; }
	virtual void					SetWalkPower(float value) { m_fWalkAccel = value; }
	virtual float GetImmunity(ALife::EHitType hit_type) { return m_ArtefactHitImmunities.GetHitImmunity(hit_type); }
	virtual void SetImmunity(ALife::EHitType hit_type, float val) { m_ArtefactHitImmunities.SetHitImmunity(hit_type, val); }

	virtual float ArtAffectHit(float power, ALife::EHitType hit_type) { return m_ArtefactHitImmunities.AffectHit(power, hit_type); }

	virtual float							AdditionalInventoryWeight() const { return m_additional_weight; }

	LPCSTR PS_bone													()		{return m_sParticlesBone.c_str(); };
	bool has_detector_visibling;
protected:
	virtual void					UpdateCLChild					()		{};
	virtual void					CreateArtefactActivation			();

	SArtefactActivation*			m_activationObj;
	SArtefactDetectorsSupport*		m_detectorObj;

	u16								m_CarringBoneID;
	shared_str						m_sParticlesName;
	shared_str						m_sParticlesBone;
	ref_light						m_pTrailLight;
	Fcolor							m_TrailLightColor;
	float							m_fTrailLightRange;
	u8								m_af_rank;
	bool							m_bLightsEnabled;
	float							m_additional_weight;

	virtual void					UpdateLights					();
public:
	IC u8							GetAfRank						() const		{return m_af_rank;}
	IC bool							CanBeActivated					()				{return m_bCanSpawnZone;};
	void							ActivateArtefact				();
	void							FollowByPath					(LPCSTR path_name, int start_idx, Fvector magic_force);
	bool							CanBeInvisible					();
	void							SwitchVisibility				(bool);

	void							SwitchAfParticles				(bool bOn);
	virtual void					StartLights();
	virtual void					StopLights();

	virtual void					PhDataUpdate					(float step);
	virtual void					PhTune							(float step)	{};

	bool							m_bCanSpawnZone;
	float							m_fHealthRestoreSpeed;
	float 							m_fRadiationRestoreSpeed;
	float 							m_fSatietyRestoreSpeed;
	float							m_fPowerRestoreSpeed;
	float							m_fBleedingRestoreSpeed;
	float							m_fJumpSpeed;
	float							m_fWalkAccel;
	CHitImmunity 					m_ArtefactHitImmunities;
public:
	enum EAFHudStates {
		eActivating = eLastBaseState+1,
	};
	virtual void					Interpolate			();

	virtual	void					PlayAnimIdle		();
	virtual void					MoveTo(Fvector const & position);
	virtual void					StopActivation		();

	virtual void					ForceTransform		(const Fmatrix& m);

	virtual void					Hide				();
	virtual void					Show				();
	virtual	void					UpdateXForm			();
	virtual bool					Action				(u16 cmd, u32 flags);
	virtual void					OnStateSwitch		(u32 S);
	virtual void					OnAnimationEnd		(u32 state);
	virtual bool					IsHidden			()	const	{return GetState()==eHidden;}

	// optimization FAST/SLOW mode
	u32						o_render_frame				;
	BOOL					o_fastmode					;
	IC void					o_switch_2_fast				()	{
		if (o_fastmode)		return	;
		o_fastmode			= TRUE	;
		//processing_activate		();
	}
	IC void					o_switch_2_slow				()	{
		if (!o_fastmode)	return	;
		o_fastmode			= FALSE	;
		//processing_deactivate		();
	}

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

struct SArtefactDetectorsSupport
{
	CArtefact*						m_parent;
	ref_sound						m_sound;

	Fvector							m_path_moving_force;
	u32								m_switchVisTime;
	const CPatrolPath*				m_currPatrolPath;
	const CPatrolPath::CVertex*		m_currPatrolVertex;
	Fvector							m_destPoint;

	LPCSTR							det_show_particles;
	LPCSTR							det_hide_particles;
	LPCSTR							det_show_snd;
	LPCSTR							det_hide_snd;
	LPCSTR							particles_bone;

			SArtefactDetectorsSupport		(CArtefact* A);
			~SArtefactDetectorsSupport		();
	void	SetVisible						(bool);
	void	Load							(LPCSTR section);
	void	FollowByPath					(LPCSTR path_name, int start_idx, Fvector force);
	void	UpdateOnFrame					();
	void	Blink							();
};
