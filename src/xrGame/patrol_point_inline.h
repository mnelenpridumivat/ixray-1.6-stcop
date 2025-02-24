#include "patrol_point.h"
////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_point_inline.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol point inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const Fvector &CPatrolPoint::position						() const
{
	VERIFY				(m_initialized);
	return				(m_position);
}

IC	const u32 &CPatrolPoint::flags								() const
{
	VERIFY				(m_initialized);
	return				(m_flags);
}

IC	const shared_str &CPatrolPoint::name						() const
{
	VERIFY				(m_initialized);
	return				(m_name);
}

inline void CPatrolPoint::set_name(shared_str name)
{
	m_name = name;
}

inline void CPatrolPoint::set_position(Fvector position)
{
	m_position = position;
}

inline void CPatrolPoint::set_flags(u32 flags)
{
	m_flags = flags;
}

inline void CPatrolPoint::set_level_vertex(u32 level_vertex_id)
{
	m_level_vertex_id = level_vertex_id;
}

inline void CPatrolPoint::set_game_vertex(GameGraph::_GRAPH_ID game_vertex_id)
{
	m_game_vertex_id = game_vertex_id;
}

IC	const u32 &CPatrolPoint::level_vertex_id					(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph) const
{
	VERIFY				(m_initialized);
#ifdef DEBUG
	verify_vertex_id	(level_graph,cross,game_graph);
#endif
	return				(m_level_vertex_id);
}

IC	const GameGraph::_GRAPH_ID &CPatrolPoint::game_vertex_id	(const ILevelGraph *level_graph, const IGameLevelCrossTable *cross, const IGameGraph *game_graph) const
{
	VERIFY				(m_initialized);
#ifdef DEBUG
	verify_vertex_id	(level_graph,cross,game_graph);
#endif
	return				(m_game_vertex_id);
}

#ifdef DEBUG
IC	void CPatrolPoint::path										(const CPatrolPath *path)
{
	VERIFY				(path);
	VERIFY				(!m_path);
	m_path				= path;
}
#endif
