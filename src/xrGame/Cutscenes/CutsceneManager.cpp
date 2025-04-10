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
#include "CutsceneItem.h"
#include "../xrEngine/xr_input.h"
#include "../xrUI/ui_base.h"

CCutsceneManager& CCutsceneManager::GetInstance()
{
	static CCutsceneManager manager;
	return manager;
}

void CCutsceneManager::PlayCutscene(LPCSTR section)
{
	auto& Self = GetInstance();
	R_ASSERT3(!Self.m_pCurrentCutscene, "Attempt to start new cutscene while other plays!", Self.m_pCurrentCutscene->GetName());
	R_ASSERT2(section && strlen(section), "Invalid cutscene section name!");
	Self.m_pCurrentCutscene = new CCutsceneItem();
	Self.m_pCurrentCutscene->Construct(section);
}

void CCutsceneManager::Update()
{
#ifndef MASTER_GOLD
	static shared_str PrevSection;
	static bool PrevAjust = false;
	if (Adjust) {
		{
			CGameFont* F = UI().Font().pFontDI;
			F->SetAligment(CGameFont::alCenter);
			F->OutSetI(0.f, -0.8f);
			F->SetColor(0xffffffff);
			F->OutNext("Adjust cutscene [%s] deviation", AdjustCutsceneSection.c_str());
			F->OutNext("Press X, Y or Z to change value on corresponding axis");
			F->OutNext("Press LSHIFT to move in opposite direction");
			F->OutNext("Press LALT to move faster");
		}
		if (PrevSection != AdjustCutsceneSection) {
			if (HudModel) {
				::Render->model_Delete(HudModel);
				HudModel = nullptr;
				HudModelKinematics = nullptr;
				HudModelKinematicsAnimated = nullptr;
			}
			if (pSettings->section_exist(AdjustCutsceneSection)) {
				PrevSection = AdjustCutsceneSection;
				PlayCutscene(PrevSection.c_str());
				AdjustDeviation = pSettings->r_fvector3(PrevSection, "deviation");
			}
		}
		if (HudModel) {
			AdjustDeviation.add(GetAdjustDelta());
		}
	}
	else if (PrevAjust) {
		PrevSection = "";
		if (HudModel) {
			::Render->model_Delete(HudModel);
			HudModel = nullptr;
			HudModelKinematics = nullptr;
			HudModelKinematicsAnimated = nullptr;
		}
	}
	PrevAjust = Adjust;
#endif
	if (m_pCurrentCutscene) {
		bool bHud = ::Render->get_HUD();
		::Render->set_HUD(false);
		Fmatrix	m_transform;
		m_transform.identity();
#ifndef MASTER_GOLD
		if (Adjust) {
			m_transform.c = AdjustDeviation;
		}
		else {
			if(auto Pivot = m_pCurrentCutscene->GetPivotObject();Pivot)
			{
				m_transform = Pivot->XFORM();
			}
		}
#else
		if(auto Pivot = m_pCurrentCutscene->GetPivotObject();Pivot)
		{
			m_transform = Pivot->XFORM();
		}
#endif

		
		m_pCurrentCutscene

		::Render->set_HUD(bHud);
	}
}

#ifndef MASTER_GOLD
void CCutsceneManager::SaveAdjust()
{
	string_path fname;
	FS.update_path(fname, "$game_config$", pSettings->r_string("cutscene_file", "file"));
	auto pCutsceneTemp = new CInifile(fname, false);
	pCutsceneTemp->w_fvector3(AdjustCutsceneSection.c_str(), "deviation", AdjustDeviation);
	pCutsceneTemp->save_as(fname);
	CInifile::Destroy(pCutsceneTemp);
}

void CCutsceneManager::ResetAdjust()
{
	AdjustDeviation = pSettings->r_fvector3(AdjustCutsceneSection, "deviation");
}

Fvector CCutsceneManager::GetAdjustDelta()
{
	Fvector Delta = { 0, 0, 0 };
	char DeltaSign = 1;
	float Step = 0.001f;
	if (pInput->iGetAsyncKeyState(SDL_SCANCODE_LALT)) {
		Step = 0.01f;
	}
	if (pInput->iGetAsyncKeyState(SDL_SCANCODE_LSHIFT))
	{
		DeltaSign = -1;
	}
	if (pInput->iGetAsyncKeyState(SDL_SCANCODE_X))
	{ //strict by X
		Delta.x += DeltaSign * Step;
	}
	if (pInput->iGetAsyncKeyState(SDL_SCANCODE_Y))
	{ //strict by Y
		Delta.y += DeltaSign * Step;
	}
	if (pInput->iGetAsyncKeyState(SDL_SCANCODE_Z))
	{ //strict by Z
		Delta.z += DeltaSign * Step;
	}
	return Delta;
}
#endif
