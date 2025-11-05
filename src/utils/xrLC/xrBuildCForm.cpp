#include "StdAfx.h"
#include "Build.h"
#include "../xrLC_Light/xrMU_Model.h"
#include "../xrLC_Light/xrMU_Model_Reference.h"

#include "../xrLC_Light/xrLC_GlobalData.h"
#include "../xrLC_Light/xrFace.h"

#include "../../xrCore/Collision/xrCDB.h"
#include "../xrLC_Light/embree_raytracing/EmbreeGeometryBuilder.h"

int GetVertexIndex(Vertex* F)
{
	vecVertexIt it = std::lower_bound(lc_global_data()->g_vertices().begin(), lc_global_data()->g_vertices().end(), F);

	R_ASSERT(it != lc_global_data()->g_vertices().end());

	return int(it - lc_global_data()->g_vertices().begin());
}

int getCFormVID(vecVertex& V, Vertex* F)
{
	vecVertexIt it = std::lower_bound(V.begin(), V.end(), F);
	return int(it - V.begin());
}
int bCriticalErrCnt = 0;

void TestEdge(Vertex* V1, Vertex* V2, Face* parent)
{
	Face* found = 0;
	int		f_count = 0;

	for (vecFaceIt I = V1->m_adjacents.begin(); I != V1->m_adjacents.end(); ++I)
	{
		Face* test = *I;
		if (test == parent) continue;
		if (test->VContains(V2))
		{
			++f_count;
			found = test;
		}
	}
	if (f_count > 1)
	{
		++bCriticalErrCnt;
		pBuild->err_multiedge().w_fvector3(V1->P);
		pBuild->err_multiedge().w_fvector3(V2->P);
	}
}

extern void SimplifyCFORM		(CDB::CollectorPacked& CL);
void CBuild::BuildCForm	()
{
	// Collecting data
	Status("CFORM: creating...");
	vecFace* cfFaces = new vecFace();
	vecVertex* cfVertices = new vecVertex();
	{
		xr_vector<bool>			cfVertexMarks;
		cfVertexMarks.assign(lc_global_data()->g_vertices().size(), false);

		Status("Sorting...");
		std::sort(lc_global_data()->g_vertices().begin(), lc_global_data()->g_vertices().end());

		Status("Collecting faces...");
		cfFaces->reserve(lc_global_data()->g_faces().size());
		for (vecFaceIt I = lc_global_data()->g_faces().begin(); I != lc_global_data()->g_faces().end(); ++I)
		{
			Face* F = *I;
			if (F->Shader().flags.bCollision)
			{
				cfFaces->push_back(F);
				int index = GetVertexIndex(F->v[0]);
				cfVertexMarks[index] = true;

				index = GetVertexIndex(F->v[1]);
				cfVertexMarks[index] = true;

				index = GetVertexIndex(F->v[2]);
				cfVertexMarks[index] = true;
			}
		}

		Status("Collecting vertices...");
		cfVertices->reserve(lc_global_data()->g_vertices().size());
		std::ranges::sort(*cfFaces);

		for (u32 V = 0; V < lc_global_data()->g_vertices().size(); V++)
		{
			if (cfVertexMarks[V])
			{
				cfVertices->push_back(lc_global_data()->g_vertices()[V]);
			}
		}
	}

	float	p_total = 0;
	float	p_cost = 1.f / (cfVertices->size());

	Fbox BB; BB.invalidate();
	for (vecVertexIt it = cfVertices->begin(); it != cfVertices->end(); it++)
	{
		BB.modify((*it)->P);
	}

	// CForm
	Status("Items to process: %d", cfFaces->size());

	p_total = 0;
	p_cost = 1.f / (cfFaces->size());

	// Collect faces
	CDB::CollisionPacked CL(BB, (int)cfVertices->size(), (int)cfFaces->size());
	//CDB::CollectorPacked CL(BB, (int)cfVertices->size(), (int)cfFaces->size());
	{
		auto& StaticCL = CL.GetStaticData();
		for (vecFaceIt F = cfFaces->begin(); F != cfFaces->end(); F++)
		{
			Face* T = *F;

			TestEdge(T->v[0], T->v[1], T);
			TestEdge(T->v[1], T->v[2], T);
			TestEdge(T->v[2], T->v[0], T);

			StaticCL.add_face(
				T->v[0]->P, T->v[1]->P, T->v[2]->P,
				T->dwMaterialGame, materials()[T->dwMaterial].sector, T->sm_group
			);
			Progress(p_total += p_cost);		// progress
		}

		if (bCriticalErrCnt) {
			err_save();
			clMsg("MultipleEdges: %d faces", bCriticalErrCnt);
		}
		
		if (g_params().m_quality != ebqDraft)
		{
			SimplifyCFORM(StaticCL);
		}
	}
	xr_delete(cfFaces);
	xr_delete(cfVertices);

	// Models
	Status("Models...");
	for (u32 ref = 0; ref < mu_refs().size(); ref++)
	{
		Progress(float(ref) / float(mu_refs().size()));
		auto MURef = mu_refs()[ref];
		auto MUObjData = CL.GetMUObjectData(MURef->model);
		if (!MUObjData)
		{
			Fbox MUBB; MUBB.invalidate();
			for (auto v : MURef->model->m_vertices)
			{
				MUBB.modify(v->P);
			}
			MUObjData = CL.CreateMUObjectData(
				MURef->model,
				MUBB,
				MURef->model->m_vertices.size(),
				MURef->model->m_faces.size());
			MURef->model->export_cform_game(*MUObjData);
			if (g_params().m_quality != ebqDraft)
			{
				SimplifyCFORM(*MUObjData);
			}
		}
		CL.AddMUInstance(MURef->model, {MURef->xform, MURef->sector});
	}

	// bb?
	//BB.invalidate();
	//for (size_t it = 0; it < CL.getVS(); it++)
	//	BB.modify(CL.getV()[it]);

	// Saving
	string_path		fn;
	IWriter* MFS = FS.w_open(xr_strconcat(fn, pBuild->path, "level.cform"));
	Status("Saving...");

	// Header
	hdrCFORM hdr;
	hdr.version = CFORM_Versions::WITH_INSTANCING;
	//hdr.vertcount = (u32)CL.getVS(); // No need
	//hdr.facecount = (u32)CL.getTS(); // No need
	hdr.aabb = BB;
	{
		MFS->open_chunk(CFORM_Chunks::Header);
		MFS->w(&hdr, sizeof(hdr));
		MFS->close_chunk();
		Msg("CFORM Saving HDR: %u", MFS->tell());
	}
	{
		MFS->open_chunk(CFORM_Chunks::StaticGeom);
		auto& StaticGeom = CL.GetStaticData();
		
		R_ASSERT(StaticGeom.getVS() <= std::numeric_limits<u32>::max(), "Too many vertices in static geom, collision is invalid!");
		MFS->w_u32(StaticGeom.getVS());
		MFS->w(StaticGeom.getV(), StaticGeom.getVS() * sizeof(Fvector));
		
		R_ASSERT(StaticGeom.getTS() <= std::numeric_limits<u32>::max(), "Too many faces in static geom, collision is invalid!");
		MFS->w_u32(StaticGeom.getTS());
		MFS->w(StaticGeom.getT(), StaticGeom.getTS() * sizeof(CDB::TRI));
		
		MFS->close_chunk();
	}
	{
		MFS->open_chunk(CFORM_Chunks::Instances);

		auto& MUObjects = CL.GetMUObjectsDataRaw();
		MFS->w_u32(MUObjects.size());
		for (auto& Obj : MUObjects)
		{
			auto& Geom = Obj.second;
		
			R_ASSERT(Geom.getVS() <= std::numeric_limits<u32>::max(), "Too many vertices in MU object geom, collision is invalid!");
			MFS->w_u32(Geom.getVS());
			MFS->w(Geom.getV(), Geom.getVS() * sizeof(Fvector));
		
			R_ASSERT(Geom.getTS() <= std::numeric_limits<u32>::max(), "Too many faces in MU object geom, collision is invalid!");
			MFS->w_u32(Geom.getTS());
			MFS->w(Geom.getT(), Geom.getTS() * sizeof(CDB::TRI));
			
		}

		MFS->close_chunk();
	}
	{
		MFS->open_chunk(CFORM_Chunks::InstanceRefs);

		auto& MUInstances = CL.GetMUInstancesDataRaw();
		//MFS->w_u32(MUInstances.size()); // amount of elements in this map are same to MUObjects
		for (auto& group : MUInstances)
		{
			MFS->w_u32(group.second.size());
			for (auto& instance : group.second)
			{
				MFS->w(&instance.transform, sizeof(instance.transform));
				MFS->w_u16(instance.sector);
			}
		}

		MFS->close_chunk();
	}

	// Clear pDeflector (it is stored in the same memory space with dwMaterialGame)
	for (auto face : lc_global_data()->g_faces())
	{
		face->pDeflector = nullptr;
	}
	FS.w_close(MFS);
}

void CBuild::BuildPortals(IWriter& fs)
{
	fs.w_chunk(fsL_PORTALS, &*portals.begin(), (u32)portals.size() * sizeof(b_portal));
}
