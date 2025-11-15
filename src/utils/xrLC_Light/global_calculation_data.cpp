#include "stdafx.h"
#include "global_calculation_data.h"

#include "../Shader_xrLC.h"

global_claculation_data	gl_data;

template <class T>
void transfer(const char *name, xr_vector<T> &dest, IReader& F, u32 chunk)
{
	IReader*	O	= F.open_chunk(chunk);
	u32		count	= O?(O->length()/sizeof(T)):0;
	clMsg			("* %16s: %d",name,count);
	if (count)  
	{
		dest.reserve(count);
		dest.insert	(dest.begin(), (T*)O->pointer(), (T*)O->pointer() + count);
	}
	if (O)		O->close	();
}

extern u32*		Surface_Load	(char* name, u32& w, u32& h);
extern void		Surface_Init	();

// 

// INTEL SELECTION
#include "embree_raytracing/EmbreeRayTrace.h"
#include "../xrForms/CompilersUI.h"
void global_claculation_data::xrLoad()
{
	string_path					N;
	FS.update_path				( N, "$game_data$", "shaders_xrlc.xr" );
	g_shaders_xrlc				= new Shader_xrLC_LIB ();
	g_shaders_xrlc->Load		( N );

	// Load CFORM
	{
		FS.update_path			(N,"$level$","build.cform");
		IReader*			fs = FS.r_open("$level$","build.cform");

		xrPhysX::CformBuilder builder;
		builder.LoadCFORM_build(*fs);

		{
			auto& StaticGeom = builder.GetStaticMesh();
			auto view = StaticGeom.GetPureTriangles();
			xr_vector<::CDB::TRI> tris(view.begin(), view.end());
			RCAST_Model.AddUniqueStaticGeom(StaticGeom.GetVertices(), tris);
			
			auto& EmbreeStaticGeom = EmbreeMain.Data.StaticGeom.geom;
			EmbreeStaticGeom.verts_v = StaticGeom.GetVertices();
			EmbreeStaticGeom.faces_v.resize(tris.size());
			for (u32 i = 0; i < tris.size(); i++)
			{
				EmbreeStaticGeom.faces_v[i].point1 = tris[i].verts[0];
				EmbreeStaticGeom.faces_v[i].point2 = tris[i].verts[1];
				EmbreeStaticGeom.faces_v[i].point3 = tris[i].verts[2];
			}
		}
		{
			auto& MUs = builder.GetMUSlots();
			auto& EmbreePrototypes = EmbreeMain.Data.InstancesContainer;
			auto& EmbreeInstances = EmbreeMain.Data.InstancesMatrices;
			for (auto& MU : MUs)
			{
				auto& PrototypeData = MU.GetPrototypeData();
				auto view = PrototypeData.GetPureTriangles();
				xr_vector<::CDB::TRI> tris(view.begin(), view.end());
				RCAST_Model.AddInstances(PrototypeData.GetVertices(), tris, MU.GetInstances());

				void* Index = (void*)&MU;
				auto& EmbreeProtSlot = EmbreePrototypes.try_emplace(Index).first->second;
				auto& EmbreeInstancesSlot = EmbreeInstances.try_emplace(Index).first->second;

			
				EmbreeProtSlot.geom.verts_v = PrototypeData.GetVertices();
				EmbreeProtSlot.geom.faces_v.resize(tris.size());
				for (u32 i = 0; i < tris.size(); i++)
				{
					EmbreeProtSlot.geom.faces_v[i].point1 = tris[i].verts[0];
					EmbreeProtSlot.geom.faces_v[i].point2 = tris[i].verts[1];
					EmbreeProtSlot.geom.faces_v[i].point3 = tris[i].verts[2];
				}

				EmbreeInstancesSlot.reserve(MU.GetInstances().size());
				for (auto& elem : MU.GetInstances())
				{
					EmbreeInstancesSlot.emplace_back(elem.transform);
				}
				
			}
		}
		RCAST_Model.Finalize();
		EmbreeMain.InitEmbreeDetails();

		FS.r_close(fs);

		LevelBB.set			(builder.GetAABB());
		
		
		//R_ASSERT			(fs->find_chunk(0));
		/*hdrCFORM			H;
		{
			auto fs_header = fs->open_chunk(CFORM_Chunks::Header);
			fs_header->r(&H,sizeof(hdrCFORM));
			R_ASSERT(CFORM_Versions::WITH_INSTANCING==H.version);
			fs_header->close();
		}
		{
			auto fs_static = fs->open_chunk(CFORM_Chunks::StaticGeom);

			xr_vector<Fvector> vertices(fs_static->r_u64());
			
			fs_static->close();
		}
		
		Fvector*	verts	= (Fvector*) fs->pointer();
		CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
		
		// Embree Loader
 		EmbreeMain.build_data.build_fcnt	 = H.facecount;
		EmbreeMain.build_data.build_vcnt	 = H.vertcount;
  		EmbreeMain.build_data.build_verts.clear();
		EmbreeMain.build_data.build_verts.resize(H.vertcount);
  		EmbreeMain.build_data.build_faces.clear();
		EmbreeMain.build_data.build_faces.resize(H.facecount);

		for (u32 Vid = 0; Vid < H.vertcount; Vid++)
		{
			EmbreeMain.build_data.build_verts[Vid] = verts[Vid];
		}
		for (u32 Tid = 0; Tid < H.facecount; Tid++)
		{
			EmbreeMain.build_data.build_faces[Tid] = tris[Tid];
		}
		Phase("Loading RCast CDB...");

		RCAST_Model.build(verts, H.vertcount, tris, H.facecount);
		clMsg("* Level CFORM: %dK", RCAST_Model.memory() / 1024);
  
		g_rc_faces.resize	(H.facecount);
		R_ASSERT(fs->find_chunk(1));
		fs->r				(&*g_rc_faces.begin(),g_rc_faces.size()*sizeof(b_rc_face));*/
	}

	EmbreeMain.InitEmbreeDetails();

	Phase("Loading build...");

 	slots_data.Load( );

 	// Lights
	{
		IReader*			fs = FS.r_open("$level$","build.lights");
		IReader*			F;	u32 cnt; R_Light* L;

		// rgb
		F		=			fs->open_chunk		(0);
		cnt		=			F->length()/sizeof(R_Light);
		L		=			(R_Light*)F->pointer();
		g_lights.rgb.assign	(L,L+cnt);
		F->close			();

		// hemi
		F		=			fs->open_chunk		(1);
		cnt		=			F->length()/sizeof(R_Light);
		L		=			(R_Light*)F->pointer();
		g_lights.hemi.assign(L,L+cnt);
		F->close			();

		// sun
		F		=			fs->open_chunk		(2);
		cnt		=			F->length()/sizeof(R_Light);
		L		=			(R_Light*)F->pointer();
		g_lights.sun.assign	(L,L+cnt);
		F->close			();

		FS.r_close			(fs);
	}

	
	// Load level data
	{
		IReader*	fs		= FS.r_open ("$level$","build.prj");
		IReader*	F;

		// Version
		u32 version;
		fs->r_chunk			(EB_Version,&version);
		R_ASSERT(XRCL_CURRENT_VERSION==version);

		// Header
		fs->r_chunk			(EB_Parameters,&g_params);

		// Load level data
		transfer("materials",	g_materials,			*fs,		EB_Materials);
		transfer("shaders_xrlc",g_shader_compile,		*fs,		EB_Shaders_Compile);
		post_process_materials( *g_shaders_xrlc, g_shader_compile, g_materials );
		// process textures

		Status			("Processing textures...");
		{
			Surface_Init		();
			F = fs->open_chunk	(EB_Textures);

#ifdef _M_X64
			u32 tex_count = F->length() / sizeof(b_texture64);
#else
			u32 tex_count = F->length() / sizeof(b_texture);
#endif			

			bool is_thm_missing = false;
			bool is_tga_missing = false;

			for (u32 t=0; t<tex_count; t++)
			{
				Progress		(float(t)/float(tex_count));

#ifdef _M_X64
				b_texture64	TEX;
				F->r(&TEX, sizeof(TEX));

				b_BuildTexture	BT;

				// ptr should be copied separately
				CopyMemory(&BT, &TEX, sizeof(TEX) - 4);
				BT.pSurface = nullptr;
#else
				b_texture TEX;
				F->r(&TEX, sizeof(TEX));

				b_BuildTexture BT;
				CopyMemory(&BT, &TEX, sizeof(TEX));
#endif

				// load thumbnail
				LPSTR N			= BT.name;
				if (strchr(N,'.')) *(strchr(N,'.')) = 0;
				_strlwr			(N);

				if (0==xr_strcmp(N,"level_lods"))
				{
					// HACK for merged lod textures
					BT.dwWidth	= 1024;
					BT.dwHeight	= 1024;
					BT.bHasAlpha= TRUE;
					BT.pSurface	= 0;
					BT.SetHasSurface(FALSE);
				}
				else
				{
					xr_strcat(N,sizeof(BT.name),".thm");
					IReader* THM = FS.r_open("$game_textures$",N);

					if (!THM)
					{
						clMsg("cannot find thm: %s", N);
						is_thm_missing = true;
						BT.bHasAlpha = false;
					}
					else {

						// version
						u32 version = 0;
						R_ASSERT(THM->r_chunk(THM_CHUNK_VERSION,&version));
						// if( version!=THM_CURRENT_VERSION )	FATAL	("Unsupported version of THM file.");

						// analyze thumbnail information
						R_ASSERT(THM->find_chunk(THM_CHUNK_TEXTUREPARAM));
						THM->r(&BT.THM.fmt, sizeof(STextureParams::ETFormat));
						BT.THM.flags.assign(THM->r_u32());
						BT.THM.border_color = THM->r_u32();
						BT.THM.fade_color = THM->r_u32();
						BT.THM.fade_amount = THM->r_u32();
						BT.THM.mip_filter = THM->r_u32();
						BT.THM.width = THM->r_u32();
						BT.THM.height = THM->r_u32();
						BOOL			bLOD = FALSE;
						if (N[0] == 'l' && N[1] == 'o' && N[2] == 'd' && N[3] == '\\') bLOD = TRUE;

						// load surface if it has an alpha channel or has "implicit lighting" flag
						BT.dwWidth				= BT.THM.width;
						BT.dwHeight				= BT.THM.height;
						BT.bHasAlpha			= BT.THM.HasAlphaChannel();
						BT.pSurface				= 0;
						BT.SetHasSurface(FALSE);

						if (!bLOD)
						{
							if (BT.bHasAlpha || BT.THM.flags.test(STextureParams::flImplicitLighted))
							{
								clMsg("- loading: %s",N);
								u32 w=0, h=0;
								BT.pSurface = Surface_Load(N,w,h);
								BT.SetHasSurface(TRUE);

								if (!BT.pSurface)
								{
									clMsg("cannot find tga texture: %s", N);
									is_tga_missing = true;
									BT.bHasAlpha = false;
								}
								else
								{
									if ((w != BT.dwWidth) || (h != BT.dwHeight)){
										Msg("! THM doesn't correspond to the texture: %dx%d -> %dx%d", BT.dwWidth, BT.dwHeight, w, h);
										BT.dwWidth = w;
										BT.dwHeight = h;
									}
									BT.Vflip	();
								}
							}
						}
					}
				}

				// save all the stuff we've created
				g_textures.push_back	(BT);
			}

			R_ASSERT2(!is_thm_missing, "Some of required thm's are missing. See log for details.");
			R_ASSERT2(!is_tga_missing, "Some of required tga_textures are missing. See log for details.");
		}
	}
}

  