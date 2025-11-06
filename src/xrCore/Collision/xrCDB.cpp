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
	tree		= 0;
	tris		= 0;
	tris_count	= 0;
	verts		= 0;
	verts_count	= 0;
	status		= S_INIT;
}

MODEL::~MODEL()
{
	syncronize();		// maybe model still in building
	status = S_INIT;
	xr_delete(tree);

	xr_free(tris);
	tris_count = 0;

	xr_free(verts);
	verts_count = 0;
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

void MODEL::build_static_geom(const Fvector* V, size_t Vcnt, const TRI* T, size_t Tcnt)
{
	R_ASSERT(S_INIT == status);
	verify_collsion(V, Vcnt, T, Tcnt);

	build_internal(StaticGeom, V, Vcnt, T, Tcnt);
	
}

void MODEL::build_mu_model(u32 id, const Fvector* V, size_t Vcnt, const TRI* T, size_t Tcnt)
{
	auto It = MUModels.find(id);
	if (!I_ASSERT(It == MUModels.end()))
	{
		return;
	}
	It = MUModels.emplace(id, raw_geom{}).first;

	build_internal(It->second, V, Vcnt, T, Tcnt);
}

void MODEL::add_instance(u32 id, const Fmatrix& Transform)
{
	auto It = MUInstances.try_emplace(id).first;
	It->second.push_back(Transform);
}

void MODEL::build(const Fvector* V, size_t Vcnt, const TRI* T, size_t Tcnt, build_callback* bc, void* bcp, void* pRW, bool RWMode)
{
	R_ASSERT(S_INIT == status);
	verify_collsion(V, Vcnt, T, Tcnt);

	build_internal(StaticGeom, V, Vcnt, T, Tcnt, bc, bcp, pRW, RWMode);
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
	if (bc)
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

	CreateNewTree(RWMode ? nullptr : (IWriter*)pRW);
}

void MODEL::finish_building(build_callback* bc, void* bcp, void* pRW, bool RWMode)
{
	// TODO: Update build_callback
	
	// callback
	if (bc)
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

	CreateNewTree(RWMode ? nullptr : (IWriter*)pRW);
	status = S_READY;
}


void CDB::MODEL::CreateNewTree(IWriter* pCache)
{
	// Release data pointers
	status = S_BUILD;

	// Allocate temporary "OPCODE" tris + convert tris to 'pointer' form
	u32* temp_tris = xr_alloc<u32>(tris_count * 3);
	if (0 == temp_tris) {
		xr_free(verts);
		xr_free(tris);
		return;
	}
	u32* temp_ptr = temp_tris;
	for (size_t i = 0; i < tris_count; i++)
	{
		*temp_ptr++ = tris[i].verts[0];
		*temp_ptr++ = tris[i].verts[1];
		*temp_ptr++ = tris[i].verts[2];
	}

	// Build a non quantized no-leaf tree
	OPCODECREATE OPCC;

	OPCC.mIMesh = new MeshInterface();
	OPCC.mIMesh->SetNbTriangles(tris_count);
	OPCC.mIMesh->SetNbVertices(verts_count);
	OPCC.mIMesh->SetPointers((IceMaths::IndexedTriangle*)temp_tris, (IceMaths::Point*)verts);
	OPCC.mSettings.mRules = SplittingRules::SPLIT_SPLATTER_POINTS | SplittingRules::SPLIT_GEOM_CENTER;
	OPCC.mNoLeaf = true;
	OPCC.mQuantized = false;

	tree = new CDB_Model(); // Sometimes, there is Opcode::Model instead CDB_Model, sometimes this object is NULL, but passes all asserts. WTF!?
	LPCSTR debug_type_name = typeid(*tree).name();
	if (IVERIFY(tree) && IVERIFY(tree->GetTree()) && !tree->Build(OPCC))
	{
		xr_free(verts);
		xr_free(tris);
		xr_free(temp_tris);
		return;
	};

	// Write cache
	if (pCache)
	{
		IWriter* pWritter = (IWriter*)(pCache);
		tree->Store(pWritter);
		FS.w_close(pWritter);
	}

	// Free temporary tris
	xr_free(temp_tris);
}

u32 MODEL::memory	()
{
	if (S_BUILD==status)	{ Msg	("! xrCDB: model still isn't ready"); return 0; }
	u32 V					= verts_count*sizeof(Fvector);
	u32 T					= tris_count *sizeof(TRI);
	return tree->GetUsedBytes()+V+T+sizeof(*this)+sizeof(*tree);
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
