////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph.h
//	Created 	: 18.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game graph class
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../../xrGame/game_graph.h"

class CGameGraphEditor:
	public IGameGraph
{
public:
	CGameGraphEditor();
	virtual ~CGameGraphEditor();
	virtual	 void set_current_level(u32 level_id);
	void realloc(const CHeader&new_header);
	void clear();
	bool empty()const;
	void set_cross_table(IGameLevelCrossTable*cross_table);
	IC const CVertex* vertex(u32 vertex_id) const { VERIFY(vertex_id < m_nodes.size()); return &m_nodes[vertex_id].vertex; }
	IC		 CVertex* vertex(u32 vertex_id)			{ VERIFY(vertex_id < m_nodes.size()); return &m_nodes[vertex_id].vertex; };
	IC		 CEdge* edge(u32 vertex_id) { VERIFY(vertex_id < m_nodes.size()); VERIFY(m_nodes[vertex_id].edges.size()); return &*m_nodes[vertex_id].edges.begin(); };
	IC		 void	add_edge(u32 vertex_id, const CEdge& edge) {VERIFY(vertex_id < m_nodes.size()); m_nodes[vertex_id].edges.push_back(edge); };
	virtual		void					save(IWriter& stream);

};
