#include "stdafx.h"
#include "CutsceneManager.h"
#include "KinematicsAnimated.h"
#include "Kinematics.h"
#include "animation_motion.h"
#include <Level.h>
#include "debug_renderer.h"
#include "../xrCore/_matrix.h"
#include <Actor.h>
#include "ActorEffector.h"

CCutsceneManager& CCutsceneManager::GetInstance()
{
	static CCutsceneManager manager;
	return manager;
}

void CCutsceneManager::PlayCutscene(LPCSTR section)
{
	auto& Self = GetInstance();
	if (Self.HudModelKinematics) {
		Msg("Attempt to start new cutscene while other plays!");
		return;
	}
	VERIFY(section && strlen(section));
	Self.HudModel = ::Render->model_Create(pSettings->r_string(section, "model"));
	Self.HudModelKinematics = smart_cast<IKinematics*>(Self.HudModel);
	Self.HudModelKinematicsAnimated = smart_cast<IKinematicsAnimated*>(Self.HudModel);
	Self.HudModelKinematics->LL_SetBonesVisibleAll();
	MotionID M2 = Self.HudModelKinematicsAnimated->ID_Cycle_Safe(pSettings->r_string(section, "anim"));
	if (bDebug) {
		Msg("playing item animation [%s]", pSettings->r_string(section, "anim"));
	}

	R_ASSERT3(M2.valid(), "model has no motion [idle] ", pSettings->r_string(section, "anim"));

	u16 pc = Self.HudModelKinematicsAnimated->partitions().count();
	for (u16 pid = 0; pid < pc; ++pid)
	{
		CBlend* B = Self.HudModelKinematicsAnimated->PlayCycle(pid, M2, true);
		R_ASSERT(B);
	}
}

void CCutsceneManager::Update()
{
	if (HudModel) {
		bool bHud = ::Render->get_HUD();
		::Render->set_HUD(false);
		Fmatrix	m_transform;
		m_transform.identity();
		HudModelKinematics->CalculateBones(true);

		::Render->set_Transform(&m_transform);
		::Render->add_Visual(HudModel, true, true);
		::Render->set_HUD(bHud);
	}
}
