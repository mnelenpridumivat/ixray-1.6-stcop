#include "stdafx.h"

#include "xr_area.h"
#define ENGINE_API
#include "../xrEngine/xr_object.h"
#include "../xrEngine/xrLevel.h"
#include "../xrEngine/xr_collide_form.h"

#ifdef	DEBUG
static bool _cdb_bDebug = false;
XRCORE_API bool *cdb_bDebug = &_cdb_bDebug;
bool bDebug()
{
	return !!(*cdb_bDebug);
}
#endif

namespace CObjectSpaceThreadData
{
	thread_local xr_vector<ISpatialShared> r_spatial; // TODO: How to remove...?
	//thread_local collide::rq_results r_temp;
}

IC int	CObjectSpace::GetNearest(xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object)
{
	return GetNearest
	(
		CObjectSpaceThreadData::r_spatial,
		q_nearest,
		point,
		range,
		ignore_object
	);
}

using namespace	collide;

//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------
CObjectSpace::CObjectSpace()
#ifdef DEBUG
	: m_pRender(0)
#endif
{
#ifdef DEBUG
	if (RenderFactory)
		m_pRender = new FactoryPtr<IObjectSpaceRender>();
#endif
	m_BoundingVolume.invalidate();
}

//----------------------------------------------------------------------
CObjectSpace::~CObjectSpace()
{
#ifdef DEBUG
	xr_delete(m_pRender);
#endif
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
int CObjectSpace::GetNearest(xr_vector<ISpatialShared>& q_spatial, xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object)
{
	q_spatial.resize(0);
	// Query objects
	q_nearest.resize(0);

	Fsphere Q;	
	Q.set(point, range);

	Fvector B;	
	B.set(range, range, range);

	g_SpatialSpace->q_box(q_spatial, 0, STYPE_COLLIDEABLE, point, B);

	// Iterate
	auto it = q_spatial.begin();
	auto end = q_spatial.end();
	for (; it != end; it++)
	{
		CObject* O = (*it)->dcast_CObject();
		if (0 == O)
			continue;

		if (O == ignore_object)	
			continue;

		Fsphere mS = { O->SpatialComponent->spatial.sphere.P, O->SpatialComponent->spatial.sphere.R };
		if (Q.intersect(mS))
			q_nearest.push_back(O);
	}

	return (int)q_nearest.size();
}

//----------------------------------------------------------------------
IC int CObjectSpace::GetNearest(xr_vector<CObject*>& q_nearest, ICollisionForm* obj, float range)
{
	CObject* O = obj->Owner();
	return GetNearest(q_nearest, O->SpatialComponent->spatial.sphere.P, range + O->SpatialComponent->spatial.sphere.R, O);
}

//----------------------------------------------------------------------
/*void CObjectSpace::Load(CDB::build_callback build_callback)
{
	Load("$level$", "level.cform", build_callback);
}

void CObjectSpace::Load(LPCSTR path, LPCSTR fname, CDB::build_callback build_callback) // to remove
{
	IReader* F = FS.r_open(path, fname);
	R_ASSERT(F);
	Load(F, build_callback);
}*/

void CObjectSpace::Load(IReader& F, CDB::build_callback build_callback)
{
	string_path LevelName = {};
	u32 crc = crc32(F.pointer(), F.length());
	auto LevelPath = FS.get_path("$level$")->m_Add;
	IReader* pReaderCache = nullptr;

	if (LevelPath != nullptr)
	{
		xr_strconcat(LevelName, "level_cache\\", LevelPath, "cform.cache"); 
		//pReaderCache = CDB::GetModelCache(LevelName, crc);
	}

	// TODO: Collision cache not implemented now
	//if (pReaderCache)
	//{
	//	Restore(*pReaderCache);
	//	FS.r_close(pReaderCache);
	//} else
	{
		xrPhysX::CformBuilder builder;
		builder.LoadCFORM_level(F);
		Create(builder);
	}

	


	
	
	
	/*hdrCFORM H;

	// Cache for cform
	string_path LevelName = {};
	u32 crc = crc32(F->pointer(), F->length());
	auto LevelPath = FS.get_path("$level$")->m_Add;
	IReader* pReaderCache = nullptr;

	if (LevelPath != nullptr)
	{
		xr_strconcat(LevelName, "level_cache\\", LevelPath, "cform.cache"); 
		pReaderCache = CDB::GetModelCache(LevelName, crc);
	}

	F->r(&H, sizeof(hdrCFORM));
	Fvector* verts = (Fvector*)F->pointer();
	CDB::TRI* tris = (CDB::TRI*)(verts + H.vertcount);
	
	if (pReaderCache != nullptr)
	{
		// Just restore
		Create(verts, tris, H, build_callback, pReaderCache, true);
	}
	else
	{
		IWriter* pWriterCache = FS.w_open("$app_data_root$", LevelName);
		pWriterCache->w_u32(crc);
		Create(verts, tris, H, build_callback, pWriterCache, false);
	}
	
	FS.r_close(F);*/
}

/*void CObjectSpace::Create(Fvector* verts, CDB::TRI* tris, const hdrCFORM& H, CDB::build_callback build_callback, void* pRW, bool RWMode)
{
	switch (H.version)
	{
	case CFORM_Versions::VANILLA:
		{
			Static.AddUniqueStaticGeom({verts, H.vertcount}, {tris, H.facecount});
			Static.Finalize();
			// TODO: Don't forget about other args!
			//Static.build(verts, H.vertcount, tris, H.facecount, build_callback, nullptr, pRW, RWMode);

			m_BoundingVolume.set(H.aabb);

			g_SpatialSpace->initialize(m_BoundingVolume);
			g_SpatialSpacePhysic->initialize(m_BoundingVolume);
			break;
		}
	case CFORM_Versions::WITH_INSTANCING:
		{
			FATAL("Not implemented");
			break;
		}
	default: NODEFAULT;
	}
}*/

void CObjectSpace::Create(const xrPhysX::CformBuilder& builder)
{
	{
		auto& StaticGeom = builder.GetStaticMesh();
		auto view = builder.GetStaticMesh().GetPureTriangles();
		xr_vector<CDB::TRI>	faces(view.begin(), view.end());
		Static.AddUniqueStaticGeom(StaticGeom.GetVertices(), faces);
	}
	{
		auto& MUs = builder.GetMUSlots();
		for (auto& MU : MUs)
		{
			auto& Prototype = MU.GetPrototypeData();
			auto view = Prototype.GetPureTriangles();
			xr_vector<CDB::TRI>	faces(view.begin(), view.end());
			Static.AddInstances(Prototype.GetVertices(), faces, MU.GetInstances());
		}
	}
	Static.Finalize();
}

void CObjectSpace::Restore(IReader& R)
{
	
}

bool CObjectSpace::RayTest(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt,
	collide::ray_cache* cache, CObject* ignore_object)
{
	collide::rq_results r_temp;
	
	xrPhysX::CDB::RayTraceOptions options;
	options.SetStart(start);
	options.SetDir(dir);
	options.r_range = range;
	options.options = xrPhysX::CDB::TraceOptions::only_first;
	xrPhysX::CDB::TraceResult result;

	if (tgt & rqtDyn)
	{
		collide::ray_defs	Q(start, dir, range, CDB::OPT_ONLYFIRST, tgt);

		u32 d_flags = STYPE_COLLIDEABLE | ((tgt & rqtObstacle) ? STYPE_OBSTACLE : 0) | ((tgt & rqtShape) ? STYPE_SHAPE : 0);

		// traverse object database
		g_SpatialSpace->q_ray(CObjectSpaceThreadData::r_spatial, 0, d_flags, start, dir, range);

		// Determine visibility for dynamic part of scene
		for (u32 o_it = 0; o_it < CObjectSpaceThreadData::r_spatial.size(); o_it++)
		{
			ISpatial* spatial = CObjectSpaceThreadData::r_spatial[o_it].get();
			CObject* collidable = spatial->dcast_CObject();
			if (collidable && (collidable != ignore_object) && collidable->collidable.model)
			{
				ECollisionFormType tp = collidable->collidable.model->Type();
				if ((tgt & (rqtObject | rqtObstacle)) && (tp == cftObject) && collidable->collidable.model->_RayQuery(Q, r_temp))
				{
					return true;
				}

				if ((tgt & rqtShape) && (tp == cftShape) && collidable->collidable.model->_RayQuery(Q, r_temp))
				{
					return true;
				}
			}
		}
	}
	
	if (tgt & rqtStatic)
	{
		Static.RayTrace(options, result);
		return !result.results.empty();
	}
	return false;
}

bool CObjectSpace::RayPick(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt,
	collide::rq_result& R, CObject* ignore_object)
{
	collide::rq_results r_temp;
	
	bool ret = false;
	// static test
	if (tgt & rqtStatic)
	{
		xrPhysX::CDB::RayTraceOptions options;
		options.SetStart(start);
		options.SetDir(dir);
		options.r_range = range;
		options.options = xrPhysX::CDB::TraceOptions::only_nearest|xrPhysX::CDB::TraceOptions::cull;
		xrPhysX::CDB::TraceResult result;
		Static.RayTrace(options, result);
		for (const auto& elem : result.results)
		{
			ret = true;
			R.set_if_less(elem.data, elem.verts, elem.range);
		}
	}

	// dynamic test
	if (tgt & rqtDyn)
	{
		collide::ray_defs Q(start, dir, R.range, CDB::OPT_ONLYNEAREST | CDB::OPT_CULL, tgt);
		// traverse object database
		u32			d_flags = STYPE_COLLIDEABLE | ((tgt & rqtObstacle) ? STYPE_OBSTACLE : 0) | ((tgt & rqtShape) ? STYPE_SHAPE : 0);
		g_SpatialSpace->q_ray(CObjectSpaceThreadData::r_spatial, 0, d_flags, start, dir, range);
		// Determine visibility for dynamic part of scene

		for (u32 o_it = 0; o_it < CObjectSpaceThreadData::r_spatial.size(); o_it++) 
		{
			ISpatial* spatial = CObjectSpaceThreadData::r_spatial[o_it].get();
			CObject* collidable = spatial->dcast_CObject();
			if (0 == collidable)				continue;
			if (collidable == ignore_object)	continue;
			ECollisionFormType tp = collidable->collidable.model->Type();
			if (((tgt & (rqtObject | rqtObstacle)) && (tp == cftObject)) || ((tgt & rqtShape) && (tp == cftShape)))
			{
				u32 C = color_xrgb(64, 64, 64);
				Q.range = R.range;

				if (collidable->collidable.model->_RayQuery(Q, r_temp)) 
				{
					C = color_xrgb(128, 128, 196);
					ret = true;
					R.set_if_less(*r_temp.r_begin());
				}
#ifdef DEBUG
				if (bDebug())
				{
					Fsphere	S;		S.P = spatial->spatial.sphere.P; S.R = spatial->spatial.sphere.R;
					(*m_pRender)->dbgAddSphere(S, C);
				}
#endif
			}
		}
	}
	return ret;
}

bool CObjectSpace::RayQuery(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb,
	LPVOID user_data, collide::test_callback* tb, CObject* ignore_object)
{
	// initialize query
	dest.r_clear		();
	collide::rq_results r_temp;

	rq_target	s_mask	=	rqtStatic;
	rq_target	d_mask	=	rq_target(	((rq.tgt&rqtObject)	?rqtObject:rqtNone		)|
										((rq.tgt&rqtObstacle)?rqtObstacle:rqtNone	)|
										((rq.tgt&rqtShape)	?rqtShape:rqtNone)		);
	u32			d_flags =	STYPE_COLLIDEABLE|((rq.tgt&rqtObstacle)?STYPE_OBSTACLE:0)|((rq.tgt&rqtShape)?STYPE_SHAPE:0);

	// Test static
	if (rq.tgt & s_mask)
	{
		xrPhysX::CDB::RayTraceOptions options;
		options.SetStart(rq.start);
		options.SetDir(rq.dir);
		options.r_range = rq.range;
		options.options = (xrPhysX::CDB::TraceOptions)rq.flags; // TODO: Remove cast and unify enums
		xrPhysX::CDB::TraceResult result;
		Static.RayTrace(options, result);

		for (const auto& elem : result.results)
		{
			r_temp.append_result(elem.data, elem.verts, elem.range, (bool)(options.options&xrPhysX::CDB::TraceOptions::only_nearest));
		}
	}
	// Test dynamic
	if (rq.tgt & d_mask) 
	{
		// Traverse object database
		g_SpatialSpace->q_ray(CObjectSpaceThreadData::r_spatial, 0, d_flags, rq.start, rq.dir, rq.range);

		for (u32 o_it = 0; o_it < CObjectSpaceThreadData::r_spatial.size(); o_it++) 
		{
			CObject* collidable = CObjectSpaceThreadData::r_spatial[o_it]->dcast_CObject();
			if (0 == collidable)
			{
				continue;
			}

			if (collidable == ignore_object)
			{
				continue;
			}

			if (ICollisionForm* cform = collidable->collidable.model)
			{
				ECollisionFormType tp = cform->Type();
				if (((rq.tgt & (rqtObject | rqtObstacle)) && (tp == cftObject)) || ((rq.tgt & rqtShape) && (tp == cftShape))) {
					if (tb && !tb(rq, collidable, user_data))
					{
						continue;
					}
					cform->_RayQuery(rq, r_temp);
				}
			}
		}
	}

	if (r_temp.r_count()) 
	{
		r_temp.r_sort();
		for (auto& elem : r_temp.r_results())
		{
			dest.append_result(elem);
			if (!(cb ? cb(elem, user_data) : TRUE))
			{
				return dest.r_count();
			}
			if (rq.flags & (CDB::OPT_ONLYNEAREST | CDB::OPT_ONLYFIRST))
			{
				return dest.r_count();
			}
		}
	}

	return dest.r_count();
}

bool CObjectSpace::RayQuery(collide::rq_results& dest, ICollisionForm* target, const collide::ray_defs& rq)
{
	VERIFY(target);
	dest.r_clear();
	return target->_RayQuery(rq, dest);
}

bool CObjectSpace::BoxQuery(Fvector const& box_center, Fvector const& box_z_axis, Fvector const& box_y_axis,
	Fvector const& box_sizes, xr_vector<Fvector>& out_tris)
{
	Fvector z_axis = box_z_axis;
	z_axis.normalize();
	Fvector y_axis = box_y_axis;
	y_axis.normalize();
	Fvector x_axis;
	x_axis.crossproduct(box_y_axis, box_z_axis).normalize();

	Fplane planes[6];
	enum { left_plane, right_plane, top_plane, bottom_plane, front_plane, near_plane };

	planes[left_plane].build(box_center - (x_axis * (box_sizes.x * 0.5f)), -x_axis);
	planes[right_plane].build(box_center + (x_axis * (box_sizes.x * 0.5f)), x_axis);
	planes[top_plane].build(box_center + (y_axis * (box_sizes.y * 0.5f)), y_axis);
	planes[bottom_plane].build(box_center - (y_axis * (box_sizes.y * 0.5f)), -y_axis);
	planes[front_plane].build(box_center - (z_axis * (box_sizes.z * 0.5f)), -z_axis);
	planes[near_plane].build(box_center + (z_axis * (box_sizes.z * 0.5f)), z_axis);

	CFrustum	frustum;
	frustum.CreateFromPlanes(planes, sizeof(planes) / sizeof(planes[0]));

	FATAL("Not implemented!");
	
	return false;

	/*CObjectSpaceThreadData::xrc.frustum_options(CDB::OPT_FULL_TEST);
	CObjectSpaceThreadData::xrc.frustum_query(&Static, frustum);

	if (out_tris)
	{
		for (CDB::RESULT* result = CObjectSpaceThreadData::xrc.r_begin();
			result != CObjectSpaceThreadData::xrc.r_end();
			++result)
		{
			out_tris->push_back(result->verts[0]);
			out_tris->push_back(result->verts[1]);
			out_tris->push_back(result->verts[2]);
		}
	}

	return !!CObjectSpaceThreadData::xrc.r_count();*/
}


//----------------------------------------------------------------------
#ifdef DEBUG
void CObjectSpace::dbgRender()
{
	(*m_pRender)->dbgRender();
}
/*
void CObjectSpace::dbgRender()
{
	R_ASSERT(bDebug);

	RCache.set_Shader(sh_debug);
	for (u32 i=0; i<q_debug.boxes.size(); i++)
	{
		Fobb&		obb		= q_debug.boxes[i];
		Fmatrix		X,S,R;
		obb.xform_get(X);
		RCache.dbg_DrawOBB(X,obb.m_halfsize,color_xrgb(255,0,0));
		S.scale		(obb.m_halfsize);
		R.mul		(X,S);
		RCache.dbg_DrawEllipse(R,color_xrgb(0,0,255));
	}
	q_debug.boxes.clear();

	for (i=0; i<dbg_S.size(); i++)
	{
		std::pair<Fsphere,u32>& P = dbg_S[i];
		Fsphere&	S = P.first;
		Fmatrix		M;
		M.scale		(S.R,S.R,S.R);
		M.translate_over(S.P);
		RCache.dbg_DrawEllipse(M,P.second);
	}
	dbg_S.clear();
}
*/
#endif
