#include "stdafx.h"
#include "game_level_cross_table_editor.h"

CGameLevelCrossTableEditor::CGameLevelCrossTableEditor()
{

}

CGameLevelCrossTableEditor::~CGameLevelCrossTableEditor()
{

}

void CGameLevelCrossTableEditor::realloc(CHeader& new_header)
{
	m_tCrossTableHeader = new_header;
	m_tpaCrossTable.resize(m_tCrossTableHeader.level_vertex_count());

}
