////////////////////////////////////////////////////////////////////////////
//	Module 		: base_client_classes.h
//	Created 	: 20.12.2004
//  Modified 	: 20.12.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay base client classes script export
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrScripts/script_export_space.h"

class DLL_Pure;
class ISpatial;
class ISheduled;
class IRenderable;
class ICollidable;
class CObject;
class IRenderVisual;
class FHierrarhyVisual;
class CBlend;
class IKinematics;
class IKinematicsAnimated;
class CPatrolPoint;

typedef class_exporter<DLL_Pure>	DLL_PureScript;
typedef class_exporter<ISheduled>	ISheduledScript;
typedef class_exporter<IRenderable>	IRenderableScript;
typedef class_exporter<ICollidable>	ICollidableScript;
typedef class_exporter<CObject>		CObjectScript;
typedef class_exporter<CBlend>	CBlendScript;
typedef class_exporter<IRenderVisual>	IRender_VisualScript;
typedef class_exporter<IKinematicsAnimated>	IKinematicsAnimatedScript;

class CPatrolPointScript {
public:
	static LPCSTR getName(CPatrolPoint*);
	static void   setName(CPatrolPoint*, LPCSTR);

	static Fvector getPosition(CPatrolPoint*);
	static void   setPosition(CPatrolPoint*, Fvector);

	static u32 getFlags(CPatrolPoint*);
	static void   setFlags(CPatrolPoint*, u32);

	static u32 getLevelVertex(CPatrolPoint*);
	static void   setLevelVertex(CPatrolPoint*, u32);

	static GameGraph::_GRAPH_ID getGameVertex(CPatrolPoint*);
	static void   setGameVertex(CPatrolPoint*, GameGraph::_GRAPH_ID);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};


class CPatrolPathScript {
	DECLARE_SCRIPT_REGISTER_FUNCTION
};