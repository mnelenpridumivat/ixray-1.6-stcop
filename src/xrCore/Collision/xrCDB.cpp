#include "stdafx.h"


#include "xrCDB.h"
#include "override/Model.h"

namespace Opcode 
{
#	include <OPC_TreeBuilders.h>
#	include <OPC_Model.h>
}

using namespace CDB;
using namespace Opcode;

XRCORE_API IReader* CDB::GetModelCache(string_path LevelName, u32 crc)
{
	IReader* pReaderCache = nullptr;

	if (FS.exist("$app_data_root$", LevelName))
	{
		pReaderCache = FS.r_open("$app_data_root$", LevelName);

		if (pReaderCache->length() <= 4 || pReaderCache->r_u32() != crc)
		{
			FS.r_close(pReaderCache);
		}
	}

	return pReaderCache;
}

// Model building
MODEL::MODEL()
{
	status		= S_INIT;
}

MODEL::~MODEL()
{
	syncronize();		// maybe model still in building
	status = S_INIT;

	xr_free(StaticGeom.tris);
	xr_free(StaticGeom.verts);

	for (auto& MU : MUModels)
	{
		xr_free(MU.verts);
		xr_free(MU.tris);
	}
}

bool MODEL::verify_collsion(const Fvector* V, size_t Vcnt, const TRI* T, size_t Tcnt)
{
	if (!I_ASSERT((Vcnt >= 4) && (Tcnt >= 2)))
	{
		if (Vcnt)
		{
			Fvector result = {0,0,0};
			for (int i = 0; i < Vcnt; i++)
			{
				result += V[i];
			}
			result /= Vcnt;
			Msg("Invalid collision face at [%f, %f, %f]", result.x, result.y, result.z);
		} else
		{
			// Never know what could happen...
			Msg("Invalid collision face: somehow there is %d faces with 0 vertices", Tcnt);
		}
		return false;
	}
	return true;
}

void MODEL::build_static_geom(Fvector* V, size_t Vcnt, TRI* T, size_t Tcnt)
{
	R_ASSERT(S_INIT == status);
	verify_collsion(V, Vcnt, T, Tcnt);

	build_internal(StaticGeom, V, Vcnt, T, Tcnt);
	
}

void MODEL::build_mu_model(u32 id, Fvector* V, size_t Vcnt, TRI* T, size_t Tcnt)
{
	if (!IVERIFY(id == MUModels.size()))
	{
		if (id > MUModels.size())
		{
			FATAL("MU model id out of range");
			return;
		}
		return;
	}
	MUModels.emplace_back(T, Tcnt, V, Vcnt);
	auto& MU = MUModels.back();

	build_internal(MU, V, Vcnt, T, Tcnt);
}

void MODEL::add_instance(u32 id, const Fmatrix& Transform)
{
	if (id > MUModels.size())
	{
		FATAL("MU model id out of range");
		return;
	}
	if (id == MUModels.size())
	{
		MUModels.push_back({});	
	}
	auto& cont = MUInstances[id];
	cont.push_back(Transform);
}

void MODEL::build(const Fvector* V, size_t Vcnt, const TRI* T, size_t Tcnt, build_callback* bc, void* bcp, void* pRW, bool RWMode)
{
	R_ASSERT(S_INIT == status);
	verify_collsion(V, Vcnt, T, Tcnt);

	build_internal(StaticGeom, V, Vcnt, T, Tcnt, bc, bcp, pRW, RWMode);

	finish_building(bc, bcp, pRW, RWMode);
}

void MODEL::build_internal(raw_geom& GeomStorage, const Fvector* V, size_t Vcnt, const TRI* T, size_t Tcnt, build_callback* bc, void* bcp, void* pRW, bool RWMode)
{
	// verts
	GeomStorage.verts_count = (u32)Vcnt;
	GeomStorage.verts = xr_alloc<Fvector>(GeomStorage.verts_count);
	CopyMemory(GeomStorage.verts, V, GeomStorage.verts_count * sizeof(Fvector));

	// tris
	GeomStorage.tris_count = (u32)Tcnt;
	GeomStorage.tris = xr_alloc<TRI>(GeomStorage.tris_count);
	CopyMemory(GeomStorage.tris, T, GeomStorage.tris_count * sizeof(TRI));

	// TODO: Remove all below to finish_building
	
	// callback
	/*if (bc)
	{
		bc(GeomStorage.verts, Vcnt, GeomStorage.tris, Tcnt, bcp);
	}

	if (pRW != nullptr && RWMode)
	{
		IReader* pReader = (IReader*)(pRW);
		tree = new CDB_Model();

		if (tree->Restore(pReader))
		{
			Msg("* Level collision DB cache found...");
			return;
		}
		else
		{
			xr_delete(tree);
			Msg("* Level collision DB cache missing, rebuilding...");
		}
	}

	CreateNewTree(RWMode ? nullptr : (IWriter*)pRW);*/
}

void MODEL::finish_building(build_callback* bc, void* bcp, void* pRW, bool RWMode)
{
	// TODO: Update build_callback
	
	// callback
	if (bc)
	{
		bc(GeomStorage.verts, Vcnt, GeomStorage.tris, Tcnt, bcp);
	}

	bool NeedBuilding = false;

	if (pRW != nullptr && RWMode)
	{
		IReader* pReader = (IReader*)(pRW);
		
		StaticTree = xr_make_unique<CDB_Model>();
		if (StaticTree->Restore(pReader))
		{
			MUModelTrees.resize(MUModels.size());
			for (auto& Tree : MUModelTrees)
			{
				Tree = xr_make_unique<CDB_Model>();
				if (!Tree->Restore(pReader))
				{
					NeedBuilding = true;
					break;
				}
			}
		}
	} else
	{
		NeedBuilding = true;
	}

	if (NeedBuilding)
	{
		StaticTree.reset(CreateNewTree(StaticGeom));
		MUModelTrees.resize(MUModels.size());
		for (u32 i = 0; i < MUModels.size(); ++i)
		{
			auto& geom = MUModels[i];
			auto& Tree = MUModelTrees[i];
			Tree.reset(CreateNewTree(geom));
		}

		IWriter* pWritter = (IWriter*)(pRW);
		
		StaticTree->Store(pWritter);
		for (const auto& Tree : MUModelTrees)
		{
			Tree->Store(pWritter);
		}
		
		FS.w_close(pWritter);
		
	}
	
	status = S_READY;
}


CDB_Model* CDB::MODEL::CreateNewTree(raw_geom& GeomStorage)
{
	// Release data pointers
	status = S_BUILD;

	// Allocate temporary "OPCODE" tris + convert tris to 'pointer' form
	u32* temp_tris = xr_alloc<u32>(GeomStorage.tris_count * 3);
	if (0 == temp_tris) {
		xr_free(GeomStorage.verts);
		xr_free(GeomStorage.tris);
		return nullptr;
	}
	xr_c_alloc_guard<u32> temp_tris_guard(temp_tris);
	u32* temp_ptr = temp_tris;
	for (size_t i = 0; i < GeomStorage.tris_count; i++)
	{
		*temp_ptr++ = GeomStorage.tris[i].verts[0];
		*temp_ptr++ = GeomStorage.tris[i].verts[1];
		*temp_ptr++ = GeomStorage.tris[i].verts[2];
	}

	// Build a non quantized no-leaf tree
	OPCODECREATE OPCC;

	OPCC.mIMesh = new MeshInterface();
	OPCC.mIMesh->SetNbTriangles(GeomStorage.tris_count);
	OPCC.mIMesh->SetNbVertices(GeomStorage.verts_count);
	OPCC.mIMesh->SetPointers((IceMaths::IndexedTriangle*)temp_tris, (IceMaths::Point*)GeomStorage.verts);
	OPCC.mSettings.mRules = SplittingRules::SPLIT_SPLATTER_POINTS | SplittingRules::SPLIT_GEOM_CENTER;
	OPCC.mNoLeaf = true;
	OPCC.mQuantized = false;

	auto tree = new CDB_Model(); // Sometimes, there is Opcode::Model instead CDB_Model, sometimes this object is NULL, but passes all asserts. WTF!?
	LPCSTR debug_type_name = typeid(*tree).name();
	if (IVERIFY(tree) && IVERIFY(tree->GetTree()) && !tree->Build(OPCC))
	{
		xr_free(GeomStorage.verts);
		xr_free(GeomStorage.tris);
		xr_delete(tree);
		return nullptr;
	}
	return tree;

	// Write cache
	/*if (pCache)
	{
		IWriter* pWritter = (IWriter*)(pCache);
		tree->Store(pWritter);
		FS.w_close(pWritter);
	}*/
}

u32 MODEL::memory	()
{
	if (S_BUILD==status)
	{
		Msg	("! xrCDB: model still isn't ready");
		return 0;
	}

	u64 VertexMem = StaticGeom.verts_count*sizeof(Fvector);
	u64 TrisMem = StaticGeom.tris_count*sizeof(TRI);
	u64 TreeMem = 0;
	u64 SingleTreeObjMem = sizeof(*StaticTree);
	if (IVERIFY(StaticTree))
	{
		TreeMem += StaticTree->GetUsedBytes() + SingleTreeObjMem;
	}

	for (const auto& MU : MUModels)
	{
		VertexMem += MU.verts_count * sizeof(Fvector);
		TrisMem += MU.tris_count * sizeof(TRI);
	}
	for (const auto& MU : MUModelTrees)
	{
		TreeMem += SingleTreeObjMem;
		if (IVERIFY(MU))
		{
			TreeMem += MU->GetUsedBytes();
		}
	}
	
	return TreeMem+VertexMem+TrisMem+sizeof(*this);
}

// This is the constructor of a class that has been exported.
// see xrCDB.h for the class definition
COLLIDER::COLLIDER()
{ 
	ray_mode		= 0;
	box_mode		= 0;
	frustum_mode	= 0;
	obb_mode		= 0;
}

COLLIDER::~COLLIDER()
{
	r_free			();
}

RESULT& COLLIDER::r_add	()
{
	rd.push_back		(RESULT());
	return rd.back		();
}

void COLLIDER::r_free	()
{
	rd.clear();
}
