#include "stdafx.h"
#include "CFormBuilder.h"

xrPhysX::CformMesh::CFormTRI::CFormTRI(const ::CDB::TRI& tri)
{
	PureTriangle = tri;
	MaterialIndex = u16(-1);
	MaterialGameIndex = u16(-1);
	UV[0] = {};
	UV[1] = {};
	UV[2] = {};
}

xrPhysX::CformMesh::CFormTRI::CFormTRI(const ::CDB::TRI& tri, u16 material, u16 materialGame, Fvector2(& uv)[3])
{
	PureTriangle = tri;
	MaterialIndex = material;
	MaterialGameIndex = materialGame;
	UV[0] = uv[0];
	UV[1] = uv[1];
	UV[2] = uv[2];
}

void xrPhysX::CformMesh::InsertVertices(const xr_span<const Fvector>& Vectors)
{
	verts.clear();
	verts.append_range(Vectors);
}

void xrPhysX::CformMesh::ReallocateTriangles(size_t NewSize)
{
	tris.reserve(NewSize);
}

void xrPhysX::CformMesh::AddTriangle(const ::CDB::TRI& tri)
{
	VERIFY(tris.size() < tris.max_size());
	tris.emplace_back(tri);
}

void xrPhysX::CformMesh::AddTriangle(const ::CDB::TRI& tri, u16 material, u16 materialGame, Fvector2(& uv)[3])
{
	VERIFY(tris.size() < tris.max_size());
	tris.emplace_back(tri, material, materialGame, uv);
}

void xrPhysX::CformMesh::SaveLevel(IWriter& fs) const
{
	R_ASSERT(verts.size() <= std::numeric_limits<u32>::max(), "Too many vertices in static geom, collision is invalid!");
	fs.w_u32(verts.size());
	fs.w(verts.data(), verts.size() * sizeof(Fvector));
	
	R_ASSERT(tris.size() <= std::numeric_limits<u32>::max(), "Too many faces in static geom, collision is invalid!");
	fs.w_u32(tris.size());
	for (const auto& elem : tris)
	{
		fs.w(&elem.PureTriangle, sizeof(elem.PureTriangle));
	}
}

void xrPhysX::CformMesh::LoadLevel(IReader& fs)
{
	verts.resize(fs.r_u32());
	fs.r(verts.data(), verts.size() * sizeof(Fvector));
	tris.resize(fs.r_u32());
	for (auto& elem : tris)
	{
		fs.r(&elem.PureTriangle, sizeof(elem.PureTriangle));
	}
}

void xrPhysX::CformMesh::SaveBuild(IWriter& fs) const
{
	R_ASSERT(verts.size() <= std::numeric_limits<u32>::max(), "Too many vertices in static geom, collision is invalid!");
	fs.w_u32(verts.size());
	fs.w(verts.data(), verts.size() * sizeof(Fvector));
	
	R_ASSERT(tris.size() <= std::numeric_limits<u32>::max(), "Too many faces in static geom, collision is invalid!");
	fs.w_u32(tris.size());
	fs.w(tris.data(), tris.size() * sizeof(CFormTRI));
}

void xrPhysX::CformMesh::LoadBuild(IReader& fs)
{
	verts.resize(fs.r_u32());
	fs.r(verts.data(), verts.size() * sizeof(Fvector));
	tris.resize(fs.r_u32());
	fs.r(tris.data(), tris.size() * sizeof(CFormTRI));
}

void xrPhysX::CformInstance::AddInstance(const Fmatrix& transform, u16 sector)
{
    instances.emplace_back(transform, sector);
}

void xrPhysX::CformInstance::SaveLevel(IWriter& fs) const
{
	prototype.SaveLevel(fs);
	fs.w_u32(instances.size());
	for (const auto& elem : instances)
	{
		fs.w(&elem.transform, sizeof(elem.transform));
		fs.w_u16(elem.sector);
	}
}

void xrPhysX::CformInstance::LoadLevel(IReader& fs)
{
	prototype.LoadLevel(fs);
	instances.resize(fs.r_u32());
	for (auto& elem : instances)
	{
		fs.r(&elem.transform, sizeof(elem.transform));
		elem.sector = fs.r_u16();
	}
}

void xrPhysX::CformInstance::SaveBuild(IWriter& fs) const
{
	prototype.SaveBuild(fs);
	fs.w_u32(instances.size());
	for (const auto& elem : instances)
	{
		fs.w(&elem.transform, sizeof(elem.transform));
	}
}

void xrPhysX::CformInstance::LoadBuild(IReader& fs)
{
	prototype.LoadLevel(fs);
	instances.resize(fs.r_u32());
	for (auto& elem : instances)
	{
		fs.r(&elem.transform, sizeof(elem.transform));
	}
}

void xrPhysX::CformBuilder::SaveHeader(IWriter& fs) const
{
	fs.open_chunk(CFORM_Chunks::Header);
	fs.w(&hdr, sizeof(hdr));
	fs.close_chunk();
	Msg("CFORM Saving HDR: %u", fs.tell());
}

void xrPhysX::CformBuilder::LoadHeader(IReader& fs)
{
	auto fs_header = fs.open_chunk(CFORM_Chunks::Header);
	fs_header->r(&hdr, sizeof(hdr));
	fs_header->close();
}

xrPhysX::CformBuilder::CformBuilder()
{
    hdr.version = CFORM_Versions::WITH_INSTANCING;
}

xrPhysX::CformInstance& xrPhysX::CformBuilder::AddMUSlot()
{
    MU.emplace_back();
    return MU.back();
}

void xrPhysX::CformBuilder::SaveCFORM_build(IWriter& MFS) const
{
	SaveHeader(MFS);
	{
		MFS.open_chunk(CFORM_Chunks::StaticGeom);
		static_mesh.SaveBuild(MFS);
		MFS.close_chunk();
	}
	{
		MFS.open_chunk(CFORM_Chunks::Instances);
		MFS.w_u32(MU.size());
		for (const auto& elem : MU)
		{
			elem.SaveBuild(MFS);
		}
		MFS.close_chunk();
	}
}

void xrPhysX::CformBuilder::SaveCFORM_level(IWriter& MFS) const
{
	SaveHeader(MFS);
	{
		MFS.open_chunk(CFORM_Chunks::StaticGeom);
		static_mesh.SaveLevel(MFS);
		MFS.close_chunk();
	}
	{
		MFS.open_chunk(CFORM_Chunks::Instances);
		MFS.w_u32(MU.size());
		for (const auto& elem : MU)
		{
			elem.SaveLevel(MFS);
		}
		MFS.close_chunk();
	}
}

void xrPhysX::CformBuilder::LoadCFORM_build(IReader& MFS)
{
	LoadHeader(MFS);
	R_ASSERT(hdr.version == CFORM_Versions::WITH_INSTANCING);
	{
		auto fs_static = MFS.open_chunk(CFORM_Chunks::StaticGeom);
		static_mesh.LoadBuild(MFS);
		fs_static->close();
	}
	{
		auto fs_instances = MFS.open_chunk(CFORM_Chunks::Instances);
		MU.resize(fs_instances->r_u32());
		for (auto& elem : MU)
		{
			elem.LoadBuild(MFS);
		}
		fs_instances->close();
	}
}

void xrPhysX::CformBuilder::LoadCFORM_level(IReader& MFS)
{
	LoadHeader(MFS);
	R_ASSERT(hdr.version == CFORM_Versions::WITH_INSTANCING);
	{
		auto fs_static = MFS.open_chunk(CFORM_Chunks::StaticGeom);
		static_mesh.LoadLevel(MFS);
		fs_static->close();
	}
	{
		auto fs_instances = MFS.open_chunk(CFORM_Chunks::Instances);
		MU.resize(fs_instances->r_u32());
		for (auto& elem : MU)
		{
			elem.LoadLevel(MFS);
		}
		fs_instances->close();
	}
}
