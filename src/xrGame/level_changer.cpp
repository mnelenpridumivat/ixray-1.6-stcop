////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.cpp
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "level_changer.h"
#include "Hit.h"
#include "Actor.h"
#include "xrServer_Objects_ALife.h"
#include "Level.h"
#include "ai_object_location.h"
#include "ai_space.h"
#include "level_graph.h"
#include "game_level_cross_table.h"

#include "UIGameSP.h"
#include "../xrEngine/xr_collide_form.h"

xr_vector<CLevelChanger*>	g_lchangers;

CLevelChanger::~CLevelChanger	()
{
}

void CLevelChanger::Center		(Fvector& C) const
{
	XFORM().transform_tiny		(C,CFORM()->getSphere().P);
}

float CLevelChanger::Radius		() const
{
	return CFORM()->getRadius	();
}

void CLevelChanger::net_Destroy	() 
{
	inherited ::net_Destroy	();
	xr_vector<CLevelChanger*>::iterator it = std::find(g_lchangers.begin(), g_lchangers.end(), this);
	if(it != g_lchangers.end())
		g_lchangers.erase(it);
}
#define DEF_INVITATION "level_changer_invitation"

BOOL CLevelChanger::net_Spawn	(CSE_Abstract* DC) 
{
	m_entrance_time				= 0;
	m_b_enabled					= true;
	m_invite_str				= DEF_INVITATION;
	CCF_Shape *l_pShape			= new CCF_Shape(this);
	collidable.model			= l_pShape;
	
	CSE_Abstract				*l_tpAbstract = (CSE_Abstract*)(DC);
	CSE_ALifeLevelChanger		*l_tpALifeLevelChanger = smart_cast<CSE_ALifeLevelChanger*>(l_tpAbstract);
	R_ASSERT					(l_tpALifeLevelChanger);

	m_game_vertex_id			= l_tpALifeLevelChanger->m_tNextGraphID;
	m_level_vertex_id			= l_tpALifeLevelChanger->m_dwNextNodeID;
	m_position					= l_tpALifeLevelChanger->m_tNextPosition;
	m_angles					= l_tpALifeLevelChanger->m_tAngles;

	m_bSilentMode				= !!l_tpALifeLevelChanger->m_bSilentMode;
	if (ai().get_level_graph()) {
		//. this information should be computed in xrAI
		ai_location().level_vertex	(ai().level_graph().vertex(u32(-1),Position()));
		ai_location().game_vertex	(ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id());
	}

	feel_touch.clear			();
	
	for (u32 i=0; i < l_tpALifeLevelChanger->shapes.size(); ++i) {
		CSE_Shape::shape_def	&S = l_tpALifeLevelChanger->shapes[i];
		switch (S.type) {
			case 0 : {
				l_pShape->add_sphere(S.data.sphere);
				break;
			}
			case 1 : {
				l_pShape->add_box(S.data.box);
				break;
			}
		}
	}

	BOOL						bOk = inherited::net_Spawn(DC);
	if (bOk) {
		l_pShape->ComputeBounds	();
		Fvector					P;
		XFORM().transform_tiny	(P,CFORM()->getSphere().P);
		setEnabled				(TRUE);
	}
	g_lchangers.push_back		(this);
	return						(bOk);
}

void CLevelChanger::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);

	const Fsphere				&s = CFORM()->getSphere();
	Fvector						P;
	XFORM().transform_tiny		(P,s.P);
	feel_touch_update			(P,s.R);

	update_actor_invitation		();
}
#include "patrol_path.h"
#include "patrol_path_storage.h"
void CLevelChanger::feel_touch_new	(CObject *tpObject)
{
	CActor*			l_tpActor = smart_cast<CActor*>(tpObject);
	VERIFY			(l_tpActor);
	if (!l_tpActor->g_Alive())
		return;

	if (m_bSilentMode) {
		NET_Packet	p;
		p.w_begin	(M_CHANGE_LEVEL);
		p.w			(&m_game_vertex_id,sizeof(m_game_vertex_id));
		p.w			(&m_level_vertex_id,sizeof(m_level_vertex_id));
		p.w_vec3	(m_position);
		p.w_vec3	(m_angles);
		Level().Send(p,net_flags(TRUE));
		return;
	}
	Fvector			p,r;
	bool			b = get_reject_pos(p,r);
	CUIGameSP		*pGameSP = smart_cast<CUIGameSP*>(CurrentGameUI());
	if (pGameSP)
        pGameSP->ChangeLevel	(m_game_vertex_id, m_level_vertex_id, m_position, m_angles, p, r, b, m_invite_str, m_b_enabled);

	m_entrance_time	= Device.fTimeGlobal;
}

bool CLevelChanger::get_reject_pos(Fvector& p, Fvector& r)
{
		p.set(0,0,0);
		r.set(0,0,0);
//--		db.actor:set_actor_position(patrol("t_way"):point(0))
//--		local dir = patrol("t_look"):point(0):sub(patrol("t_way"):point(0))
//--		db.actor:set_actor_direction(-dir:getH())

		if(m_ini_file && m_ini_file->section_exist("pt_move_if_reject"))
		{
			LPCSTR p_name = m_ini_file->r_string("pt_move_if_reject", "path");
			const CPatrolPath*		patrol_path = ai().patrol_paths().path(p_name);
			VERIFY					(patrol_path);
			
			const CPatrolPoint*		pt;
			pt						= &patrol_path->vertex(0)->data();
			p						= pt->position();

			Fvector tmp;
			pt						= &patrol_path->vertex(1)->data();
			tmp.sub					(pt->position(),p);
			tmp.getHP				(r.y,r.x);
			return true;
		}
		return false;
}

BOOL CLevelChanger::feel_touch_contact	(CObject *object)
{
	BOOL bRes	= (((CCF_Shape*)CFORM())->Contact(object));
	bRes		= bRes && smart_cast<CActor*>(object) && smart_cast<CActor*>(object)->g_Alive();
	return		bRes;
}

void CLevelChanger::update_actor_invitation()
{
	if(m_bSilentMode)						return;
	xr_vector<CObject*>::iterator it		= feel_touch.begin();
	xr_vector<CObject*>::iterator it_e		= feel_touch.end();

	for(;it!=it_e;++it){
		CActor*			l_tpActor = smart_cast<CActor*>(*it);
		VERIFY			(l_tpActor);
		
		if(!l_tpActor->g_Alive())
			continue;

		if(m_entrance_time+5.0f < Device.fTimeGlobal){
			CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(CurrentGameUI());
			Fvector p,r;
			bool b = get_reject_pos(p,r);
			
			if(pGameSP)
				pGameSP->ChangeLevel(m_game_vertex_id,m_level_vertex_id,m_position,m_angles,p,r,b, m_invite_str, m_b_enabled);

			m_entrance_time		= Device.fTimeGlobal;
		}
	}
}

void CLevelChanger::save(NET_Packet &output_packet)
{
	inherited::save			(output_packet);
	output_packet.w_stringZ	(m_invite_str);
	output_packet.w_u8		(m_b_enabled?1:0);
}

void CLevelChanger::load(IReader &input_packet)
{
	inherited::load			(input_packet);
	input_packet.r_stringZ	(m_invite_str);
	m_b_enabled				= !!input_packet.r_u8();
}

/*void CLevelChanger::Save(CSaveObjectSave* Object) const
{
	Object->BeginChunk("CLevelChanger");
	{
		inherited::Save(Object);
		Object->GetCurrentChunk()->w_stringZ(m_invite_str);
		Object->GetCurrentChunk()->w_bool(m_b_enabled);
	}
	Object->EndChunk();
}

void CLevelChanger::Load(CSaveObjectLoad* Object)
{
	Object->BeginChunk("CLevelChanger");
	{
		inherited::Load(Object);
		Object->GetCurrentChunk()->r_stringZ(m_invite_str);
		Object->GetCurrentChunk()->r_bool(m_b_enabled);
	}
	Object->EndChunk();
}*/

void CLevelChanger::Serialize(ISaveObject& Object)
{
	Object.BeginChunk("CLevelChanger");
	{
		inherited::Serialize(Object);
		Object << m_invite_str << m_b_enabled;
	}
	Object.EndChunk();
}

BOOL CLevelChanger::net_SaveRelevant()
{
	if(!m_b_enabled || m_invite_str!=DEF_INVITATION )
		return TRUE;
	else
		return inherited::net_SaveRelevant();
}

#ifdef DEBUG_DRAW
#include "debug_renderer.h"
void CLevelChanger::OnRender() {
	if (!bDebug)
		return;
	if (!(dbg_net_Draw_Flags.is_any(dbg_draw_lchangers)))
		return;

	DRender->OnFrameEnd();
	Fvector l_half = l_half.set(0.5f, 0.5f, 0.5f);
	Fmatrix l_ball = {}, l_box = {};
	auto& l_shapes = ((CCF_Shape*)CFORM())->Shapes();

	constexpr u32 Color = color_xrgb(255, 0, 0);

	for (auto l_pShape = l_shapes.begin(); l_shapes.end() != l_pShape; ++l_pShape)
	{
		switch (l_pShape->type)
		{
		case 0:
		{
			Fsphere& l_sphere = l_pShape->data.sphere;
			l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R);
			Fvector l_p; XFORM().transform(l_p, l_sphere.P);
			l_ball.translate_add(l_p);
			Level().debug_renderer().draw_ellipse(l_ball, Color);
		}
		break;
		case 1:
		{
			l_box.mul(XFORM(), l_pShape->data.box);
			Level().debug_renderer().draw_obb(l_box, l_half, Color);
		}
		break;
		}
	}
	// Enable name draw on close dist
	if (Level().CurrentViewEntity()->Position().distance_to(XFORM().c) < 100.0f)
	{
		//DRAW name
		Fmatrix res = {};
		res.mul(Device.mFullTransform, XFORM());

		Fvector4 v_res = {};

		float delta_height = 0.f;

		// get up on 2 meters
		Fvector shift = {};
		static float gx = 0.0f;
		static float gy = 2.0f;
		static float gz = 0.0f;
		shift.set(gx, gy, gz);
		res.transform(v_res, shift);

		// check if the object in sight
		if (v_res.z < 0 || v_res.w < 0)
			return;
		if (v_res.x < -1.f || v_res.x > 1.f || v_res.y < -1.f || v_res.y>1.f)
			return;

		// get real (x,y)
		float x = (1.f + v_res.x) / 2.f * (Device.TargetWidth);
		float y = (1.f - v_res.y) / 2.f * (Device.TargetHeight) - delta_height;

		UI().Font().pFontMedium->SetColor(0xffff0000);
		UI().Font().pFontMedium->OutSet(x, y -= delta_height);
		UI().Font().pFontMedium->OutNext(Name());
	}
}
#endif // DEBUG