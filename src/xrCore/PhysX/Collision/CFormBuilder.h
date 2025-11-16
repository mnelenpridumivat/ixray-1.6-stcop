#pragma once
#include "../../../xrEngine/xrLevel.h"
#include "Collision/xrCDB.h"

namespace xrPhysX
{
    class XRCORE_API CformMesh
    {

        struct CFormTRI
        {
            ::CDB::TRI PureTriangle;
            u16 MaterialIndex;
            u16 MaterialGameIndex;
            Fvector2 UV[3];

            CFormTRI() = default;
            CFormTRI(const ::CDB::TRI& tri);
            CFormTRI(const ::CDB::TRI& tri, u16 material, u16 materialGame, Fvector2 (&uv)[3]);
        };
        
        xr_vector<Fvector> verts;
        xr_vector<CFormTRI> tris;
    public:
        void Clear();
        
        void InsertVertices(const xr_span<const Fvector>& Vectors);
        void ReallocateTriangles(size_t NewSize);
        void AddTriangle(const ::CDB::TRI& tri);
        void AddTriangle(const ::CDB::TRI& tri, u16 material, u16 materialGame, Fvector2 (&uv)[3]);

        const xr_vector<Fvector>& GetVertices() const {return verts;}
        auto GetPureTriangles() const { return tris | std::views::transform([](const CFormTRI& elem){ return elem.PureTriangle;});}

        void SaveLevel(IWriter& fs) const;
        void SaveBuild(IWriter& fs) const;
        
        void LoadLevel(IReader& fs);
        void LoadBuild(IReader& fs);
    };

    class XRCORE_API CformInstance
    {
    public:
        struct InstanceData
        {
            Fmatrix transform;
            u16 sector;
        };
    private:
        CformMesh prototype;
        xr_vector<InstanceData> instances;
    public:
        CformMesh& GetPrototypeData(){return prototype;}
        const CformMesh& GetPrototypeData() const{return prototype;}
        void AddInstance(const Fmatrix& transform, u16 sector);

        auto GetInstancesTransforms() const { return instances | std::views::transform([](const InstanceData& elem){ return elem.transform;}); }
        const xr_vector<InstanceData>& GetInstances() const {return instances;}

        void SaveLevel(IWriter& fs) const;
        void SaveBuild(IWriter& fs) const;
        
        void LoadLevel(IReader& fs);
        void LoadBuild(IReader& fs);
    };

    class XRCORE_API CformBuilder
    {
        hdrCFORM hdr;
        CformMesh static_mesh;
        xr_vector<CformInstance> MU;

        void SaveHeader(IWriter& fs) const;
        void LoadHeader(IReader& fs);
        
    public:
        CformBuilder();

        void Clear();

        IC void SetAABB(const Fbox& aabb) {hdr.aabb = aabb;}
        IC const Fbox& GetAABB() const {return hdr.aabb;}
        
        CformMesh& GetStaticMesh() {return static_mesh;}
        const CformMesh& GetStaticMesh() const {return static_mesh;}
        CformInstance& AddMUSlot();
        const xr_vector<CformInstance>& GetMUSlots() const {return MU;}

        void SaveCFORM_build(IWriter& MFS) const;
        void SaveCFORM_level(IWriter& MFS) const;

        void LoadCFORM_build(IReader& MFS);
        void LoadCFORM_level(IReader& MFS);
    };
    
}
