#include "stdafx.h"
#include "FHierrarhyVisual.h"
#if (RENDER==R_R4)
#include "r4.h"
#endif
#if (RENDER==R_R2)
#include "r2.h"
#endif
#if (RENDER==R_R1)
#include "FStaticRender.h"
#endif

int CRender::translateSector(IRender_Sector* pSector)
{
	if (!pSector)
		return -1;

	for (u32 i=0; i<Sectors.size(); ++i)
	{
		if (Sectors[i]==pSector)
			return i;
	}

	FATAL			("Sector was not found!");
	NODEFAULT;

#ifdef DEBUG
	return			(-1);
#endif // #ifdef DEBUG
}

IRender_Sector* CRender::detectSector(const Fvector& P)
{
	IRender_Sector*	S	= nullptr;	
	Fvector			dir; 

	dir.set				(0,-1,0);
	S					= detectSector(P,dir);
	if (nullptr==S)		
	{
		dir.set				(0,1,0);
		S					= detectSector(P,dir);
	}
	return S;
}
//thread_local xrXRC sectors_detect_xrc;
IRender_Sector* CRender::detectLastSector(const Fvector& P)
{
	if(SectorsCount()==1)
		return pOutdoorSector;

	auto detectSector = [&](const Fvector& P, Fvector& dir) -> IRender_Sector*
	{
		//sectors_detect_xrc.ray_options		(CDB::OPT_ONLYNEAREST);
		xrPhysX::CDB::RayTraceOptions options;
		options.options = xrPhysX::CDB::TraceOptions::only_nearest;
		// Portals model
		if (rmPortals)	
		{
			options.SetStart(P);
			options.SetDir(dir);
			options.r_range = 1000.0f;
			xrPhysX::CDB::TraceResult result;
			rmPortals->RayTrace(options, result);

			if (!result.results.empty())
			{
				const auto& Res = result.results[0];
				CPortal*	pPortal	= (CPortal*) Portals[Res.data.dummy];
				CSector* S = pPortal->getSectorFacing(P);
				FHierrarhyVisual* pV = (FHierrarhyVisual*)S->root();
				if(pV && pV->vis.box.contains(P)){
					return S;
				}
			}
			
			/*sectors_detect_xrc.ray_query	(rmPortals,P,dir,1000.f);
			if (sectors_detect_xrc.r_count()) {
				CDB::RESULT *RP = sectors_detect_xrc.r_begin();
				CDB::TRI*	pTri	= rmPortals->get_tris() + RP->id;
				CPortal*	pPortal	= (CPortal*) Portals[pTri->dummy];
				CSector* S = pPortal->getSectorFacing(P);
				FHierrarhyVisual* pV = (FHierrarhyVisual*)S->root();
				if(pV)
				{
					if(pV->vis.box.contains(P))
						return S;
				}
			}*/
		}

		// Geometry model
		options.SetStart(P);
		options.SetDir(dir);
		options.r_range = 1000.0f;
		xrPhysX::CDB::TraceResult result;
		g_pGameLevel->ObjectSpace.GetStaticModel().RayTrace(options, result);
		
		if (!result.results.empty())
		{
			return getSector(result.results[0].data.sector);
		}
		/*sectors_detect_xrc.ray_query	(g_pGameLevel->ObjectSpace.GetStaticModel(),P,dir,1000.f);
		if (sectors_detect_xrc.r_count()) {
			CDB::RESULT *RP = sectors_detect_xrc.r_begin();
			return getSector(RP->sector);
		}*/

		return nullptr;
	};

	IRender_Sector*	S	= nullptr;	
	Fvector			dir; 

	dir.set				(0,-1,0);
	S					= detectSector(P,dir);
	if (nullptr==S)		
	{
		dir.set				(0,1,0);
		S					= detectSector(P,dir);
	}
	return S;

}

IRender_Sector* CRender::detectSector(const Fvector& P, Fvector& dir)
{
	if(SectorsCount()==1)
	{
		return pOutdoorSector;
	}

	xrPhysX::CDB::RayTraceOptions options;
	options.options = xrPhysX::CDB::TraceOptions::only_nearest;
	
	//sectors_detect_xrc.ray_options		(CDB::OPT_ONLYNEAREST);
	
	// Portals model
	int		id1		= -1;
	float	range1	= 500.f;
	u16 Sector1 = u16(-1);
	u32 Dummy1 = u32(-1);
	if (rmPortals)	
	{
		options.SetStart(P);
		options.SetDir(dir);
		options.r_range = range1;
		xrPhysX::CDB::TraceResult result;
		rmPortals->RayTrace(options, result);

		if (!result.results.empty())
		{
			const auto& Res = result.results[0];
			id1 = Res.id;
			range1 = Res.range;
			Sector1 = Res.data.sector;
			Dummy1 = Res.data.dummy;
		}
		
		/*sectors_detect_xrc.ray_query	(rmPortals,P,dir,range1);
		if (sectors_detect_xrc.r_count()) {
			CDB::RESULT *RP1 = sectors_detect_xrc.r_begin();
			id1 = RP1->id; range1 = RP1->range; 
		}*/
	}

	// Geometry model
	int		id2		= -1;
	float	range2	= range1;
	u16 Sector2 = u16(-1);;
	u32 Dummy2 = u32(-1);
	options.SetStart(P);
	options.SetDir(dir);
	options.r_range = range2;
	xrPhysX::CDB::TraceResult result;
	g_pGameLevel->ObjectSpace.GetStaticModel().RayTrace(options, result);

	if (!result.results.empty())
	{
		const auto& Res = result.results[0];
		id2 = Res.id;
		range2 = Res.range;
		Sector2 = Res.data.sector;
		Dummy2 = Res.data.dummy;
	}
	
	/*sectors_detect_xrc.ray_query	(g_pGameLevel->ObjectSpace.GetStaticModel(),P,dir,range2);
	if (sectors_detect_xrc.r_count()) {
		CDB::RESULT *RP2 = sectors_detect_xrc.r_begin();
		id2 = RP2->id; range2 = RP2->range;
	}*/

	// Select ID
	int ID;
	u16 Sector;
	u32 Dummy;
	if (id1>=0) {
		if (id2>=0) {
			if (range1<=range2+EPS)
			{
				ID = id1;
				Sector = Sector1;
				Dummy = Dummy1;
			} else
			{
				ID = id2;
				Sector = Sector2;
				Dummy = Dummy2;
			}
		}
		else
		{
			ID = id1;									// only id1 found
			Sector = Sector1;
			Dummy = Dummy1;
		}
	} else if (id2>=0)
	{
		ID = id2;						// only id2 found
		Sector = Sector2;
		Dummy = Dummy2;
	}
	else
	{
		return nullptr;
	}

	if (ID==id1) {
		// Take sector, facing to our point from portal
		VERIFY(Dummy != u32(-1));
		CPortal*	pPortal	= (CPortal*) Portals[Dummy];
		return pPortal->getSectorFacing(P);
	} else {
		// Take triangle at ID and use it's Sector
		VERIFY(Sector != u16(-1));
		return getSector(Sector);
	}
}

xr_vector<IRender_Sector*> CRender::detectSectors_sphere(CSector* sector, const Fvector& b_center, const Fvector& b_dim)
{
	xr_vector<IRender_Sector*> m_sectors;
	m_sectors.push_back(sector);
	if (rmPortals)
	{
		xrPhysX::CDB::AABBBoxTraceOptions options;
		options.options = xrPhysX::CDB::TraceOptions::full_test;
		options.SetAABB(Fbox().setb(b_center,b_dim));
		xrPhysX::CDB::TraceResult result;

		rmPortals->BoxTrace(options, result);

		for (const auto& elem : result.results)
		{
			CPortal* pPortal = (CPortal*) Portals[elem.data.dummy];

			if(!pPortal)
			{
				continue;
			}

			CSector *pFront = pPortal->Front();
			CSector *pBack = pPortal->Back();

			if(pFront)
			{
				m_sectors.push_back(pFront);
			}

			if(pBack)
			{
				m_sectors.push_back(pBack);
			}
		}
		
		/*sectors_detect_xrc.box_options(CDB::OPT_FULL_TEST);
		sectors_detect_xrc.box_query(rmPortals,b_center,b_dim);
		for (int K=0; K< sectors_detect_xrc.r_count(); K++)
		{
			CPortal* pPortal = (CPortal*) Portals[rmPortals->get_tris()[sectors_detect_xrc.r_begin()[K].id].dummy];

			if(!pPortal)
				continue;

			CSector *pFront = pPortal->Front();
			CSector *pBack = pPortal->Back();

			if(pFront)
				m_sectors.push_back(pFront);

			if(pBack)
				m_sectors.push_back(pBack);
		}*/
	}
	return m_sectors;
}

xr_vector<IRender_Sector*> CRender::detectSectors_frustum(CSector* sector, CFrustum* _frustum)
{
	xr_vector<IRender_Sector*> m_sectors;
	m_sectors.push_back(sector);
	if (rmPortals)
	{
		xrPhysX::CDB::FrustumTraceOptions options;
		options.options = xrPhysX::CDB::TraceOptions::full_test;
		FATAL("Not implemented");
		xrPhysX::CDB::TraceResult result;

		rmPortals->FrustumTrace(options, result);

		for (const auto& elem : result.results)
		{
			CPortal* pPortal = (CPortal*) Portals[elem.data.dummy];

			if(!pPortal)
			{
				continue;
			}

			CSector *pFront = pPortal->Front();
			CSector *pBack = pPortal->Back();

			if(pFront)
			{
				m_sectors.push_back(pFront);
			}

			if(pBack)
			{
				m_sectors.push_back(pBack);
			}
		}
		
		/*sectors_detect_xrc.frustum_options(CDB::OPT_FULL_TEST);
		sectors_detect_xrc.frustum_query(rmPortals,*_frustum);
		for (int K=0; K< sectors_detect_xrc.r_count(); K++)
		{
			CPortal* pPortal = (CPortal*) Portals[rmPortals->get_tris()[sectors_detect_xrc.r_begin()[K].id].dummy];

			if(!pPortal)
				continue;

			CSector *pFront = pPortal->Front();
			CSector *pBack = pPortal->Back();

			if(pFront)
				m_sectors.push_back(pFront);

			if(pBack)
				m_sectors.push_back(pBack);
		}*/
	}
	return m_sectors;
}