#include "stdafx.h"
#include "game_graph_editor.h"

CGameGraphEditor::CGameGraphEditor()
{
	m_current_level_cross_table = nullptr;
}

CGameGraphEditor::~CGameGraphEditor()
{

}

void CGameGraphEditor::set_current_level(u32 level_id)
{
	R_ASSERT(level_id == 0);
}

void CGameGraphEditor::realloc(const CHeader& new_header)
{
	m_header = new_header;
	m_nodes.clear();
	m_nodes.resize(new_header.m_vertex_count);
	m_current_level_some_vertex_id = _GRAPH_ID(-1);
	m_enabled.assign(header().vertex_count(), true);
}

void CGameGraphEditor::clear()
{

}

bool CGameGraphEditor::empty() const
{
	return m_nodes.size();
}

void CGameGraphEditor::set_cross_table(IGameLevelCrossTable* cross_table)
{
	// TODO: Potentially memory leak
	xr_delete(m_current_level_cross_table);
	m_current_level_some_vertex_id = 0;
	m_current_level_cross_table = cross_table;
}

void CGameGraphEditor::save(IWriter& stream)
{
	m_header.save(&stream);

	for (auto& elem : m_nodes) {
		elem.Serialize(stream);
	}

	stream.w_u32(header().levels().size());
	for (auto& elem : header().levels()) {
		stream.w(&elem.first, sizeof(elem.first));
		stream.w(&m_current_level_cross_table->header(), sizeof(IGameLevelCrossTable::CHeader));
		for (size_t i = 0; i < m_current_level_cross_table->header().level_vertex_count(); i++)
		{
			stream.w(&m_current_level_cross_table->vertex(i), sizeof(IGameLevelCrossTable::CCell));
		}
	}

}
