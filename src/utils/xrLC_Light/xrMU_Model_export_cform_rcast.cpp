#include "stdafx.h"
#include "xrLC_GlobalData.h"
#include "xrMU_Model.h"
#include "xrMU_Model_Reference.h"

#include "../../xrCore/Collision/xrCDB.h"
#include "../Shader_xrLC.h"

u32 convert_nax(base_Face* F);

void xrMU_Model::export_cform_game(CDB::CollectorPacked& CL)
{
	// Collecting data
	v_faces cfFaces = {};
	v_vertices cfVertices = {};
	{
		xr_vector<bool>	cfVertexMarks;
		cfVertexMarks.assign(m_vertices.size(),false);

		std::ranges::sort(m_vertices);

		// faces and mark vertices
		cfFaces.reserve	(m_faces.size());
		for (auto face : m_faces)
		{
			if (!face->Shader().flags.bCollision)
			{
				continue;
			}
			cfFaces.push_back(face);
			for (u32 vit=0; vit<3; vit++)
			{
				u32 g_id = u32(std::ranges::lower_bound(m_vertices, face->v[vit]) - m_vertices.begin());
				cfVertexMarks	[g_id] = true;
			}
		}

		// verts
		cfVertices.reserve	(m_vertices.size());
		std::ranges::sort(cfFaces);
		for (u32 V=0; V<m_vertices.size(); V++)
		{
			if (cfVertexMarks[V])
			{
				cfVertices.push_back(m_vertices[V]);
			}
		}
	}

	// Collect faces
	for (auto T : cfFaces)
	{
		CL.add_face( T->v[0]->P, T->v[1]->P, T->v[2]->P, T->dwMaterialGame, -1, T->sm_group);
	}
}

void xrMU_Model::export_cform_rcast	(CDB::CollectorPacked& CL)
{
	for (auto& face : m_faces)
	{
		face->flags.bProcessed = false;
	}
	
	v_faces adjacent;
	adjacent.reserve(6*2*3);

	for (auto face : m_faces)
	{
		const Shader_xrLC&	SH		= face->Shader();
		if (!SH.flags.bLIGHT_CastShadow)
		{
			continue;
		}
		adjacent.clear	();
		for (int vit=0; vit<3; vit++)
		{
			_vertex* V	= face->v[vit];
			for (auto& adj : V->m_adjacents)
			{
				adjacent.push_back(adj);
			}
		}
		std::ranges::sort(adjacent);
		adjacent.erase(std::ranges::unique(adjacent).begin(),adjacent.end());
		
		bool bAlready = false;
		for (auto adj : adjacent)
		{
			if (adj==face || !adj->flags.bProcessed || face->isEqual(*adj))
			{
				continue;
			}
			bAlready = true;
		}
		
		if (!bAlready) 
		{
			face->flags.bProcessed = true;
			CL.add_face_D			(face->v[0]->P,face->v[1]->P,face->v[2]->P, convert_nax(face), face->sm_group);//
		}
	}
}

void xrMU_Model::export_cform_rcast	(CDB::CollectorPacked& CL, Fmatrix& xform)
{
	for		(u32 fit=0; fit<m_faces.size(); fit++)	m_faces[fit]->flags.bProcessed = false;

	v_faces			adjacent;	adjacent.reserve(6*2*3);

	for (v_faces_it it = m_faces.begin(); it!=m_faces.end(); it++)
	{
		_face*	F				= (*it);
		const Shader_xrLC&	SH		= F->Shader();
		if (!SH.flags.bLIGHT_CastShadow)		continue;

		// Collect
		adjacent.clear	();
		for (int vit=0; vit<3; vit++)
		{
			_vertex* V	= F->v[vit];
			for (u32 adj=0; adj<V->m_adjacents.size(); adj++)
				adjacent.push_back(V->m_adjacents[adj]);
		}

		// Unique
		std::sort		(adjacent.begin(),adjacent.end());
		adjacent.erase	(std::unique(adjacent.begin(),adjacent.end()),adjacent.end());
		BOOL			bAlready	= FALSE;
		for (u32 ait=0; ait<adjacent.size(); ait++)
		{
			_face*	Test				= adjacent[ait];
			if (Test==F)				continue;
			if (!Test->flags.bProcessed)continue;
			if (F->isEqual(*Test))
			{
				bAlready			= TRUE;
				break;
			}
		}

		//
		if (!bAlready) 
		{
			F->flags.bProcessed		= true;
			Fvector					P[3];
			xform.transform_tiny	(P[0],F->v[0]->P);
			xform.transform_tiny	(P[1],F->v[1]->P);
			xform.transform_tiny	(P[2],F->v[2]->P);
			CL.add_face_D			(P[0],P[1],P[2], convert_nax(F), F->sm_group);//
		}
	}
}

void xrMU_Model::export_cform_rcast_new(xr_vector<FaceDataIntel>& faces, Fmatrix& xform)
{
	for (v_faces_it it = m_faces.begin(); it != m_faces.end(); it++)
	{
		_face* F = (*it);
		const Shader_xrLC& SH = F->Shader();
		if (!SH.flags.bLIGHT_CastShadow) continue;

 		Fvector					P[3];
		xform.transform_tiny(P[0], F->v[0]->P);
		xform.transform_tiny(P[1], F->v[1]->P);
		xform.transform_tiny(P[2], F->v[2]->P);

		FaceDataIntel data;
		data.v1 = P[0];
		data.v2 = P[1];
		data.v3 = P[2];
		data.ptr = F;
		faces.push_back(data);
	}
}

void xrMU_Model::export_cform_rcast_new(GeomLightingContainer& container)
{
	auto& geom = container.geom;
	auto& geom_transp = container.geom_transp;

	for (const auto& elem : m_faces)
	{
		const Shader_xrLC& SH = elem->Shader();
		if (!SH.flags.bLIGHT_CastShadow)
		{
			continue;
		}
		b_material& M = inlc_global_data()->materials()[elem->dwMaterial];
		b_texture& T = inlc_global_data()->textures()[M.surfidx];
		if (elem->flags.bOpaque || !T.pSurface || !T.bHasAlpha)
		{
			geom.AddFace(elem, elem->v[0]->P, elem->v[1]->P, elem->v[2]->P);
		}
		else
		{
			geom_transp.AddFace(elem, elem->v[0]->P, elem->v[1]->P, elem->v[2]->P);
		}
	}
}

void xrMU_Model::export_cform_rcast_new(GeomShadowContainer& container)
{
	auto& geom = container.geom;

	for (const auto& elem : m_faces)
	{
		const Shader_xrLC& SH = elem->Shader();
		if (!SH.flags.bLIGHT_CastShadow)
		{
			continue;
		}
		b_material& M = inlc_global_data()->materials()[elem->dwMaterial];
		b_texture& T = inlc_global_data()->textures()[M.surfidx];
		geom.AddFace(elem, elem->v[0]->P, elem->v[1]->P, elem->v[2]->P);
	}
}

