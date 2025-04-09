#include "StdAfx.h"
#include "game_level_cross_table.h"

CGameLevelCrossTable::CGameLevelCrossTable(IReader& reader, bool VerificationMode)
{
	{
		IReader* chunk = nullptr;
		if (VerificationMode) {
			chunk = reader.open_chunk(CROSS_TABLE_CHUNK_VERSION);
			R_ASSERT2(chunk, "Cross table is corrupted!");
		}
		else {
			chunk = &reader;
		}
		
		chunk->r(&m_tCrossTableHeader, sizeof(m_tCrossTableHeader));
		if (VerificationMode) {
			chunk->close();
		}
	}
	R_ASSERT2(m_tCrossTableHeader.version() == XRAI_CURRENT_VERSION, "Cross table version mismatch!");
	{
		IReader* chunk = nullptr;
		if (VerificationMode) {
			chunk = reader.open_chunk(CROSS_TABLE_CHUNK_DATA);
			R_ASSERT2(chunk, "Cross table is corrupted!");
		}
		else {
			chunk = &reader;
		}
		m_tpaCrossTable.resize(header().level_vertex_count());
		for (u32 i = 0; i < header().level_vertex_count(); ++i) {
			chunk->r(&m_tpaCrossTable[i], sizeof(m_tpaCrossTable[i]));
		}
		if (VerificationMode) {
			chunk->close();
		}
	}
}
