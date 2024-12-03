#pragma once

#ifdef DEBUG_DRAW
#include "FVF.h"
#include "../../Include/xrRender/DebugRender.h"

class dxDebugRender : public IDebugRender
{
public:
					dxDebugRender		();

	virtual void	Render				();
	virtual void	add_lines			(Fvector const *vertices, u32 const &vertex_count, u32 const *pairs, u32 const &pair_count, u32 const &color);

	// routed to RCache
	virtual void	NextSceneMode		();
	virtual void	ZEnable				(bool bEnable);
	virtual void	OnFrameEnd			();
	virtual void	SetShader			(const debug_shader &shader);
	virtual void	CacheSetXformWorld	(const Fmatrix& M);
	virtual void	CacheSetCullMode	(CullMode);
	virtual void	SetAmbient			(u32 colour);

	// Shaders
	virtual void	SetDebugShader		(dbgShaderHandle shdHandle);
	virtual void	DestroyDebugShader	(dbgShaderHandle shdHandle);

#ifdef DEBUG_DRAW
	virtual void	dbg_DrawTRI			(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, u32 C);
#endif	//	DEBUG

private:
	enum {
		line_vertex_limit				= 32767 * 256,
		line_index_limit				= 32767 * 256
	};

public:
	struct LineRenderPack {
		float x1 = 0.0f, y1 = 0.0f;
		float x2 = 0.0f, y2 = 0.0f;
		u32 color = 0xffffffff;
	};

	xr_vector<LineRenderPack> m_lines;

private:
	ref_shader m_dbgShaders[dbgShaderCount];
};

extern dxDebugRender DebugRenderImpl;
//extern dxDebugRender* rdebug_render;
#endif // DEBUG