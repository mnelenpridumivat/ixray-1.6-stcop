#pragma once
#include <PxRigidStatic.h>

#include "PxQueryFiltering.h"
#include "PxQueryReport.h"
//#include "_stl_extensions.h"
#include "CFormBuilder.h"
#include "Collision/xrCDB.h"
#include "PhysX/PhysXCore.h"
#include "pvd/PxPvd.h"

namespace CDB
{
    class TRI;
}

namespace xrPhysX::CDB
{
    class MODEL;
    constexpr u16 SectorInvalid = u16(-1);
    
    class XRCORE_API CollisionPrototype
    {
        physx::PxTriangleMesh* Prototype;
        xr_vector<::CDB::TRIExtra> DataPerTriangle = {};
    public:
        CollisionPrototype(physx::PxTriangleMesh* mesh) : Prototype(mesh) {}
        void SetExtraData(const xr_span<const ::CDB::TRI>& data);

        auto GetPrototype() const { return Prototype; }
        const auto& GetTriangeData(u32 triangleID) const
        {
            VERIFY(DataPerTriangle.size() > triangleID);
            return DataPerTriangle[triangleID];
        }
    };

    //class StableInstanceRef;
    
    class XRCORE_API CollisionInstance {
        MODEL* CollisionModel;
        physx::PxRigidStatic* m_actor;
        //xr_unique_ptr<StableInstanceRef> stable_ref;
        u32 m_shared_mesh;
        u16 Sector = SectorInvalid;
    
    public:
        //CollisionInstance() noexcept = default;
        CollisionInstance(MODEL* model, const Fmatrix& transform, u16 sector, u32 prototype) noexcept;
        ~CollisionInstance() = default;

        //CollisionInstance(const CollisionInstance& other) noexcept = default;
        //CollisionInstance(CollisionInstance&& other) noexcept = default;
        //CollisionInstance& operator=(const CollisionInstance& other) noexcept = default;
        //CollisionInstance& operator=(CollisionInstance&& other) noexcept = default;

        physx::PxRigidStatic& GetActor() const {return *m_actor;}
        //u32 GetID() const;
        const CollisionPrototype& GetPrototype() const;
        u16 GetSector() const { return Sector; }
    };

    /*class XRCORE_API StableInstanceRef
    {
        MODEL* CollisionModel = nullptr;
        u32 ID = u32(-1);
    public:
        StableInstanceRef(MODEL* CollisionModel, u32 ID) : CollisionModel(CollisionModel), ID(ID) {}

        const CollisionInstance& GetCollisionInstance() const;
        u32 GetID() const {return ID;}
    };*/

    enum class TraceOptions : u8
    {
        cull = (1<<0),
        only_first = (1<<1),
        only_nearest = (1<<2),
        full_test = (1<<3)	
    };

    ENUM_CLASS_FLAGS(TraceOptions)

    class XRCORE_API RayTraceOptions
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

    class XRCORE_API AABBBoxTraceOptions
    {
        physx::PxVec3 center;
        physx::PxVec3 halfExtents;
    public:
        TraceOptions options;
        void SetAABB(const Fbox& value)
        {
            Fvector xrCenter, xrExtents;
            value.get_CD(xrCenter, xrExtents);
            center.x = xrCenter.x;
            center.y = xrCenter.y;
            center.z = xrCenter.z;
            halfExtents.x = xrExtents.x;
            halfExtents.y = xrExtents.y;
            halfExtents.z = xrExtents.z;
        }
        const physx::PxVec3& GetCenter() const { return center; }
        const physx::PxVec3& GetExtents() const { return halfExtents; }
    };

    class XRCORE_API OBBBoxTraceOptions
    {
        physx::PxVec3 center;
        physx::PxVec3 halfExtents;
        physx::PxQuat rotation;
    public:
        TraceOptions options;
        void SetOBB(const Fobb& value)
        {
            center.x = value.m_translate.x;
            center.y = value.m_translate.y;
            center.z = value.m_translate.z;
            halfExtents.x = value.m_halfsize.x;
            halfExtents.y = value.m_halfsize.y;
            halfExtents.z = value.m_halfsize.z;
            PhysXMathHelper::Conv_MatrixToPxQuatNoScale(rotation, value.m_rotate);
        }
        const physx::PxVec3& GetCenter() const { return center; }
        const physx::PxVec3& GetExtents() const { return halfExtents; }
        const physx::PxQuat& GetRot() const { return rotation; }
    };

    /*class xrRaycastBuffer : public physx::PxRaycastBuffer
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
    };*/
    
    class XRCORE_API TraceResult
    {
    public:
        xr_vector<::CDB::RESULT> results;
    };
    
    class XRCORE_API MODEL
    {
        xr_vector<xr_unique_ptr<CollisionPrototype>> m_prototypes;
        xr_vector<xr_unique_ptr<CollisionInstance>> m_instances;
        ::std::vector<::std::unique_ptr<int>> test;
        physx::PxScene* m_scene;
        bool ready = false;

        void AddInstances(u32 prototype, const xr_vector<CformInstance::InstanceData>& instances);
        void AddPrototype(const xr_span<const Fvector3>& vertices, const xr_span<const ::CDB::TRI>& faces);

        void ConvertHitToVertices(const physx::PxRaycastHit& hit, Fvector vertices[3]) const;
        void ConvertHitToResult(const physx::PxRaycastHit& hit, ::CDB::RESULT& result) const;

        //void ConvertHitToResult(const physx::PxOverlapHit& hit, TraceOptions options, xr_vector<::CDB::RESULT>& result);
        //void GetIntersectingTriangles(const physx::PxOverlapHit& hit, const physx::PxTriangleMesh* geom, TraceOptions options, xr_vector<::CDB::RESULT>& result);
        
        void ConvertHitsToResults(xr_span<physx::PxOverlapHit> hits, TraceOptions options, xr_vector<::CDB::RESULT>& result) const;

        void ExecuteBoxTrace(const physx::PxBoxGeometry& geom, const physx::PxTransform& transform, TraceOptions options, physx::PxQueryFlags QueryFlags, TraceResult& result) const;
    public:
        MODEL();
        ~MODEL();
        
        MODEL(const MODEL&) = delete;
        MODEL& operator=(const MODEL&) = delete;
    
        MODEL(MODEL&&) = default;
        MODEL& operator=(MODEL&&) = default;

        const CollisionPrototype& GetPrototype(u32 ID);
        const CollisionInstance& GetInstance(u32 ID);
        
        void AddUniqueStaticGeom(const xr_span<const Fvector3>& vertices, const xr_span<const ::CDB::TRI>& faces);
        void AddInstances(
            const xr_span<const Fvector3>& vertices,
            const xr_span<const ::CDB::TRI>& faces,
            const xr_vector<CformInstance::InstanceData>& Instances
            );
        u32 GetInstancesNum() const {return m_instances.size();}

        void Finalize();

        void RayTrace(const RayTraceOptions& options, TraceResult& result) const;
        void BoxTrace(const AABBBoxTraceOptions& options, TraceResult& result) const;
        void BoxTrace(const OBBBoxTraceOptions& options, TraceResult& result) const;
    };
}
