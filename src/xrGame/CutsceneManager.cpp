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

	//u16 root_id = Self.HudModelKinematics->LL_GetBoneRoot();
	//CBoneInstance& root_binst = Self.HudModelKinematics->LL_GetBoneInstance(root_id);
	//root_binst.set_callback_overwrite(true);
	//root_binst.mTransform.identity();

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
		//m_transform.i = Device.vCameraRight;
		//m_transform.j = Device.vCameraTop;// -Device.vCameraDirection;//Device.vCameraTop;
		//m_transform.k = Device.vCameraDirection;// Device.vCameraTop; //Device.vCameraDirection;
		//m_transform.c = Device.vCameraPosition;
		//::Render->set_Transform(&m_transform);
		//Actor()->Cameras().hud_camera_Matrix(m_transform);
		//auto Pos = m_transform.c;
		//m_transform.rotateX(90);
		//m_transform.c.y += 100;
		//m_transform = HudModelKinematics->LL_GetTransform(HudModelKinematics->LL_GetBoneRoot());
		//m_transform.build_camera_dir(Device.vCameraPosition, Device.vCameraDirection, Device.vCameraTop);
		::Render->set_Transform(&m_transform);
		HudModelKinematics->CalculateBones(true);
		::Render->add_Visual(HudModel, true);
		::Render->set_HUD(bHud);
		for (u16 i = 0; i < HudModelKinematics->LL_BoneCount(); ++i) {
			Fmatrix l_ball = HudModelKinematics->LL_GetTransform(i);
			auto Pos = l_ball.c;
			l_ball.scale(0.01, 0.01, 0.01);
			l_ball.c = Pos;
			Level().debug_renderer().draw_ellipse(l_ball, color_xrgb(0, 255, 255));
		}
	}
}
