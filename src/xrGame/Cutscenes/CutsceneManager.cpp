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
	auto new_item = new CCutsceneItem();
	try
	{
		new_item->Construct(section);
	} catch(...)
	{
		R_ASSERT3(false, "Failed to create cutscene item!", section);
		xr_delete(new_item);
		return;
	}
	Self.m_pCurrentCutscene = new_item;
}

void CCutsceneManager::Update()
{
#ifndef MASTER_GOLD
	static shared_str PrevCutsceneSection = nullptr;
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
			F->OutNext("Press F to play cutscene forward");
			F->OutNext("Press B to play cutscene backward");
			F->OutNext("Press S to stop cutscene");
		}
		if (PrevCutsceneSection != AdjustCutsceneSection) {
			if (m_pCurrentCutscene) {
				xr_delete(m_pCurrentCutscene);
			}
			if (pSettings->section_exist(AdjustCutsceneSection)) {
				PrevCutsceneSection = AdjustCutsceneSection;
				PlayCutscene(PrevCutsceneSection.c_str());
				AdjustDeviation = m_pCurrentCutscene->GetPivotObject()->Position();
			}
		}
		if (m_pCurrentCutscene) {
			AdjustDeviation.add(GetAdjustDelta());
		}
	}
	else if (PrevAjust) {
		PrevCutsceneSection = "";
		if (m_pCurrentCutscene) {
			xr_delete(m_pCurrentCutscene);
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

		
#ifndef MASTER_GOLD
		if (pInput->iGetAsyncKeyState(SDL_SCANCODE_B)) {
			m_pCurrentCutscene->BackwardAnimation();
		} else if (pInput->iGetAsyncKeyState(SDL_SCANCODE_S))
		{
			m_pCurrentCutscene->StopAnimation();
		} else if (pInput->iGetAsyncKeyState(SDL_SCANCODE_F))
		{
			m_pCurrentCutscene->ForwardAnimation();
		}
#endif
		m_pCurrentCutscene->Update();

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
