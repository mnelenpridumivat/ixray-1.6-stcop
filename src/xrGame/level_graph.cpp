////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph.cpp
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "level_graph.h"
#include "../xrEngine/Editor/XrEditorSceneInterface.h"

LPCSTR LEVEL_GRAPH_NAME = "level.ai";
CLevelGraph::CLevelGraph()
{
	VERIFY(Device.IsEditorMode() == false);
	string_path file_name;

#ifndef AI_COMPILER
	FS.update_path(file_name, "$level$", LEVEL_GRAPH_NAME);
#else
	strconcat(sizeof(file_name), file_name, filename, LEVEL_GRAPH_NAME);
#endif

	m_reader = FS.r_open(file_name);

	// m_header & data
	m_header = (CHeader*)m_reader->pointer();
	const u32 AIVersion = header().version();
	R_ASSERT(AIVersion >= XRAI_MINIMAL_VERSION && AIVersion <= XRAI_CURRENT_VERSION);
	m_reader->advance(sizeof(CHeader));

	switch (AIVersion)
	{
		case XRAI_MINIMAL_VERSION: // ver 10 - CS/CoP format
		{
			NodeCompressed10* Src = (NodeCompressed10*)m_reader->pointer();
			m_nodes = new CVertex[header().vertex_count()];

			for (u32 i = 0; i < header().vertex_count(); ++i)
			{


				for (u8 j = 0; j < 4; ++j)
				{
					u32 link_value = Src[i].link(j);
					m_nodes[i].UncompressedNode.link(j, link_value);
				}

				// Îñòàëüíûå ïîëÿ
				m_nodes[i].UncompressedNode.high = Src[i].high;
				m_nodes[i].UncompressedNode.low = Src[i].low;
				m_nodes[i].UncompressedNode.plane = Src[i].plane;

				m_nodes[i].UncompressedNode.p.xz(Src[i].p.xz());
				m_nodes[i].UncompressedNode.p.y(Src[i].p.y());
			}
			break;
		}
		/*case XRAI_CURRENT_VERSION: // ver 11 - 25-bit format
		{
			NodeCompressed* compressed_nodes = (NodeCompressed*)m_reader->pointer();
			m_nodes = new CVertex[header().vertex_count()];

			for (size_t i = 0; i < header().vertex_count(); ++i)
			{
				for (u8 link_idx = 0; link_idx < 4; ++link_idx)
				{
					u32 old_link = compressed_nodes[i].link(link_idx);
					m_nodes[i].UncompressedNode.link(link_idx, old_link);
				}

				m_nodes[i].UncompressedNode.high = compressed_nodes[i].high;
				m_nodes[i].UncompressedNode.low = compressed_nodes[i].low;
				m_nodes[i].UncompressedNode.plane = compressed_nodes[i].plane;

				m_nodes[i].UncompressedNode.p.xz(compressed_nodes[i].p.xz());
				m_nodes[i].UncompressedNode.p.y(compressed_nodes[i].p.y());
			}
			break;
		}*/
		case XRAI_CURRENT_VERSION:
		{
			m_nodes = (CVertex*)m_reader->pointer();
			break;
		}
	}

	m_row_length				= iFloor((header().box().max.z - header().box().min.z)/header().cell_size() + EPS_L + 1.5f);
	m_column_length				= iFloor((header().box().max.x - header().box().min.x)/header().cell_size() + EPS_L + 1.5f);
	m_access_mask.assign		(header().vertex_count(),true);
	unpack_xz					(vertex_position(header().box().max),m_max_x,m_max_z);

#ifdef DEBUG
#	ifndef AI_COMPILER
	sh_debug->create("debug\\ai_nodes", "$null");
	m_current_level_id = -1;
	m_current_actual = false;
	m_current_center = Fvector().set(flt_max,flt_max,flt_max);
	m_current_radius = Fvector().set(flt_max,flt_max,flt_max);
#	endif
#endif
}

CLevelGraph::~CLevelGraph()
{
	if (m_header->version() < XRAI_CURRENT_VERSION)
	{
		xr_delete(m_nodes);
	}

	VERIFY(Device.IsEditorMode() == false);
	FS.r_close(m_reader);
}
