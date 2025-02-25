////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_inline.h
//	Created 	: 18.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game graph inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CGameGraph::CGameGraph									(LPCSTR file_name, u32 current_version)
{
	auto m_reader						= FS.r_open(file_name);
	VERIFY							(m_reader);
	m_header.load					(m_reader);
	R_ASSERT2						(header().version() == XRAI_CURRENT_VERSION,"Graph version mismatch!");

	m_nodes.resize(header().vertex_count());
	for (auto& elem : m_nodes) {
		elem.Serialize(*m_reader);
	}

	m_current_level_some_vertex_id	= _GRAPH_ID(-1);
	m_enabled.assign				(header().vertex_count(),true);
	
	{
		auto LevelsNum = m_reader->r_u32();
		m_cross_tables.reserve(LevelsNum);
		for (u32 i = 0; i < LevelsNum; ++i) {
			_LEVEL_ID LevelID;
			LevelID = m_reader->r_u32();
			m_cross_tables.insert_or_assign(LevelID, CGameLevelCrossTable(*m_reader));
		}
	}

	m_current_level_cross_table		= 0;
}

IC CGameGraph::CGameGraph											(IReader &_stream)
{
	IReader							&stream = const_cast<IReader&>(_stream);
	m_header.load					(&stream);
	R_ASSERT2						(header().version() == XRAI_CURRENT_VERSION,"Graph version mismatch!");

	m_nodes.resize(header().vertex_count());
	for (auto& elem : m_nodes) {
		elem.Serialize(stream);
	}

	m_current_level_some_vertex_id	= _GRAPH_ID(-1);
	m_enabled.assign				(header().vertex_count(),true);
	
	{
		auto LevelsNum = header().levels().size();
		m_cross_tables.reserve(LevelsNum);
		for (u32 i = 0; i < LevelsNum; ++i) {
			_LEVEL_ID LevelID;
			_stream.r(&LevelID, sizeof(LevelID));
			m_cross_tables.insert_or_assign(LevelID, CGameLevelCrossTable(stream));
		}
	}

	m_current_level_cross_table		= 0;
}


IC	void CGameGraph::set_current_level								(u32 const level_id)
{
	VERIFY(m_cross_tables.contains(level_id));
	m_current_level_cross_table = &m_cross_tables[level_id];

	m_current_level_some_vertex_id = _GRAPH_ID(-1);
	for (_GRAPH_ID i=0, n = header().vertex_count(); i<n; ++i) {
		if (level_id != vertex(i)->level_id())
			continue;

		m_current_level_some_vertex_id	= i;
		break;
	}

	VERIFY						(valid_vertex_id(m_current_level_some_vertex_id));
}


IC void CGameGraph::save								(IWriter &stream)
{
	m_header.save				(&stream);
	for (auto& elem : m_nodes) {
		elem.Serialize(stream);
	}
	for (auto& elem : m_cross_tables) {
		stream.w_u32(elem.first);
		stream.w(&elem.second.header(), sizeof(IGameLevelCrossTable::CHeader));
		for (size_t i = 0; i < elem.second.header().level_vertex_count(); i++)
		{
			stream.w(&elem.second.vertex(i), sizeof(IGameLevelCrossTable::CCell));
		}
	}
}