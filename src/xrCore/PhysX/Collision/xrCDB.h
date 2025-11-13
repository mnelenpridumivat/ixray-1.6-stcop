#pragma once
#include <PxRigidStatic.h>

#include "PxQueryReport.h"
#include "_stl_extensions.h"
#include "Collision/xrCDB.h"
#include "pvd/PxPvd.h"

namespace CDB
{
    class TRI;
}

namespace xrPhysX::CDB
{
    class MODEL;
    constexpr u16 SectorInvalid = u16(-1);
    
    class CollisionPrototype
    {
        physx::PxTriangleMesh* Prototype;
        xr_vector<::CDB::TRIExtra> DataPerTriangle = {};
    public:
        CollisionPrototype(physx::PxTriangleMesh* mesh) : Prototype(mesh) {}
        void SetExtraData(const xr_span<const ::CDB::TRI>& data);

        auto GetPrototype() const { return Prototype; }
    };
    
    class CollisionInstance {
        MODEL* CollisionModel;
        physx::PxRigidStatic* m_actor;
        u32 m_shared_mesh;
        u16 Sector = SectorInvalid;
    
    public:
        CollisionInstance(MODEL* model, const Fmatrix& transform, u16 sector, u32 prototype);

        physx::PxRigidStatic& GetActor() const {return *m_actor;}
    };

    enum class TraceOptions : u8
    {
        cull = (1<<0),
        only_first = (1<<1),
        only_nearest = (1<<2),
        full_test = (1<<3)	
    };

    ENUM_CLASS_FLAGS(TraceOptions)

    class RayTraceOptions
    {
        physx::PxVec3 r_start;
        physx::PxVec3 r_dir;
    public:
        void SetStart(const Fvector& value)
        {
            r_start.x = value.x;
            r_start.y = value.y;
            r_start.z = value.z;
        }
        void SetDir(const Fvector& value)
        {
            r_dir.x = value.x;
            r_dir.y = value.y;
            r_dir.z = value.z;
        }
        const physx::PxVec3& GetStart() const { return r_start; }
        const physx::PxVec3& GetDir() const { return r_dir; }
        TraceOptions options;
        float r_range = 10000.f;
    };

    class xrRaycastBuffer : public physx::PxRaycastBuffer
    {
        xrPhysX::CDB::TraceOptions options;
        physx::PxRaycastHit nearestHit;

    public:

        xrRaycastBuffer(xrPhysX::CDB::TraceOptions options) : physx::PxRaycastBuffer(), options(options)
        {
            nearestHit.distance = std::numeric_limits<float>::max();
        }

    protected:
        physx::PxAgain processTouches(const physx::PxRaycastHit* buffer, physx::PxU32 nbHits) override;

    private:

        bool IsFrontFace(const physx::PxRaycastHit& hit) const;
    };
    
    class RayTraceResult
    {
    public:
        xr_vector<::CDB::RESULT> results;
    };
    
    class MODEL
    {
        xr_vector<CollisionPrototype> m_prototypes;
        xr_vector<CollisionInstance> m_instances;
        physx::PxScene* m_scene;

        void AddInstances(u32 prototype, const xr_vector<xr_pair<Fmatrix, u16>>& instances);
        void AddPrototype(const xr_span<const Fvector3>& vertices, const xr_span<const ::CDB::TRI>& faces);
    public:
        MODEL();
        ~MODEL();

        const CollisionPrototype& GetPrototype(u32 ID);
        void AddUniqueStaticGeom(const xr_span<const Fvector3>& vertices, const xr_span<const ::CDB::TRI>& faces);
        void AddInstances(
            const xr_span<const Fvector3>& vertices,
            const xr_span<const ::CDB::TRI>& faces,
            const xr_vector<xr_pair<Fmatrix, u16>>& Instances
            );

        void Finalize();

        void RayTrace(const RayTraceOptions& options, RayTraceResult& result);
    };
}
