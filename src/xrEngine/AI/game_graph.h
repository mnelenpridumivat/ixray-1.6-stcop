////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph.h
//	Created 	: 18.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game graph class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"
#include "game_level_cross_table.h"

struct ENGINE_API CVertexWithEdges {
	using CEdge = GameGraph::CEdge;
	using CVertex = GameGraph::CVertex;
	using CLevelPoint = GameGraph::CLevelPoint;

	//~CVertexWithEdges() {}

	CVertex vertex;
	xr_vector<CEdge> edges = {};
	xr_vector<CLevelPoint> death_points = {};

	void Serialize(IWriter& writer);
	void Serialize(IReader& reader);
};

class ENGINE_API IGameGraph 
{
private:
	friend class CRenumbererConverter;

public:
	using _GRAPH_ID =		GameGraph::_GRAPH_ID;
	using _LEVEL_ID =		GameGraph::_LEVEL_ID;
	using _LOCATION_ID =	GameGraph::_LOCATION_ID;
	using SLevel =			GameGraph::SLevel;
	using CEdge =			GameGraph::CEdge;
	using CVertex =			GameGraph::CVertex;
	using CHeader =			GameGraph::CHeader;
	using CLevelPoint =		GameGraph::CLevelPoint;

	using const_iterator = xr_vector<CEdge>::const_iterator;
	using LEVEL_POINT_VECTOR = xr_vector<CLevelPoint>;
	using const_spawn_iterator = xr_vector<CLevelPoint>::const_iterator;
	using ENABLED = xr_vector<bool>;

protected:
	CHeader										m_header;
	xr_vector<CVertexWithEdges>	m_nodes = {};
	LEVEL_POINT_VECTOR							m_points;
	mutable ENABLED					m_enabled;
	_GRAPH_ID						m_current_level_some_vertex_id;

protected:
	xr_hash_map<_LEVEL_ID, IGameLevelCrossTable> m_cross_tables;
	IGameLevelCrossTable			*m_current_level_cross_table;

public:

						IGameGraph();
	virtual				~IGameGraph();
	virtual		void	save						(IWriter &stream);
				bool	Search						(u32 start_vertex_id, u32 dest_vertex_id,xr_vector<u32>& OutPath,const xr_vector<GameGraph::STerrainPlace>* VertexTypes = nullptr, float MaxRange = type_max(float), u32 MaxIterationCount = 0xFFFFFFFF,u32 MaxVisitedNodeCount = 0xFFFFFFFF) const;
				bool	SearchNearestVertex			(u32 start_vertex_id,u8 LevelID,u32&Result) const;
	
	IC	const IGameLevelCrossTable	&cross_table			() const;

public:
	IC		const CHeader			&header					() const;
	IC		bool					mask					(const svector<_LOCATION_ID,GameGraph::LOCATION_TYPE_COUNT> &M, const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const;
	IC		bool					mask					(const _LOCATION_ID M[GameGraph::LOCATION_TYPE_COUNT], const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const;
	IC		float					distance				(const _GRAPH_ID tGraphID0, const _GRAPH_ID tGraphID1) const;
	IC		bool					accessible				(u32 vertex_id) const;
	IC		void					accessible				(u32 vertex_id, bool value) const;
	IC		bool					valid_vertex_id			(u32 vertex_id) const;
	IC		void					begin					(u32 vertex_id, const_iterator &start, const_iterator &end) const;
	IC		void					begin_spawn				(u32 vertex_id, const_spawn_iterator &start, const_spawn_iterator &end) const;
	IC		const _GRAPH_ID			&value					(u32 vertex_id, const_iterator &i) const;
	IC		const float				&edge_weight			(const_iterator i) const;
	IC		const CVertex			*vertex					(u32 vertex_id) const;
	IC		void					set_invalid_vertex		(_GRAPH_ID &vertex_id) const;
	IC		_GRAPH_ID				vertex_id				(const CVertex *vertex) const;
	virtual		void					set_current_level		(u32 level_id)=0;
	IC		const _GRAPH_ID			&current_level_vertex	() const;
};
#include "game_graph_inline.h"