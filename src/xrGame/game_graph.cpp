#include "StdAfx.h"
#include "game_graph.h"
CGameGraph::CGameGraph(IReader& _stream)
{
	VERIFY(!Device.IsEditorMode());
	IReader& stream = const_cast<IReader&>(_stream);
	m_header.load(&stream);
	const u32 AIVersion = header().version();
	R_ASSERT2(CHECK_SPAWN_VERSION(AIVersion), "Graph version mismatch!");
	m_nodes.resize(header().vertex_count());
	for (auto& elem : m_nodes) {
		elem.Serialize(_stream);
	}
	m_current_level_some_vertex_id = _GRAPH_ID(-1);
	m_enabled.assign(header().vertex_count(), true);

	auto LevelNum = _stream.r_u32();
	m_cross_tables.reserve(LevelNum);
	for (u32 i = 0; i < LevelNum; ++i) {
		_LEVEL_ID LevelID = _stream.r_u32();
		m_cross_tables.insert_or_assign(LevelID, CGameLevelCrossTable(_stream));
	}

	m_current_level_cross_table = 0;
}

CGameGraph::~CGameGraph()
{
	VERIFY(Device.IsEditorMode() == false);
}

void CGameGraph::set_current_level(u32  level_id)
{
	VERIFY(m_cross_tables.contains(level_id));
	m_current_level_cross_table = &m_cross_tables[level_id];

	m_current_level_some_vertex_id = _GRAPH_ID(-1);
	for (_GRAPH_ID i = 0, n = header().vertex_count(); i < n; ++i) {
		if (level_id != vertex(i)->level_id())
			continue;

		m_current_level_some_vertex_id = i;
		break;
	}

	VERIFY(valid_vertex_id(m_current_level_some_vertex_id));
}