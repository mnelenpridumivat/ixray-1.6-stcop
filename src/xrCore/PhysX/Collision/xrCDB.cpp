#include "stdafx.h"
#include "xrCDB.h"
#include "Collision/xrCDB.h"

#include "PxMaterial.h"
#include "PxScene.h"
#include "cooking/PxCooking.h"
#include "cooking/PxTriangleMeshDesc.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "extensions/PxDefaultSimulationFilterShader.h"
#include "geometry/PxTriangleMesh.h"
#include "PhysX/PhysXCore.h"

void xrPhysX::CDB::CollisionPrototype::SetExtraData(const xr_span<const ::CDB::TRI>& data)
{
    DataPerTriangle.reserve(data.size());
    for (auto& tri : data)
    {
       DataPerTriangle.emplace_back(tri.data); 
    }
}

xrPhysX::CDB::CollisionInstance::CollisionInstance(MODEL* model, const Fmatrix& transform, u16 sector, u32 prototype)
{
    CollisionModel = model;
    Sector = sector;
    m_shared_mesh = prototype;
    physx::PxTransform PxTransform = PhysXMathHelper::Conv_MatrixToPxTransform(transform);
    physx::PxVec3 pxScale = PhysXMathHelper::Conv_MatrixToPxScale(transform);

    physx::PxTriangleMeshGeometry geom(CollisionModel->GetPrototype(m_shared_mesh).GetPrototype());
    geom.scale = pxScale;

    auto& physics = PhysXInstance::GetInstance().GetPhysics();
    
    auto material = PhysXMaterialManager::GetInstance().GetDefaultMaterial(); // TODO: Make proper reading of material

    auto shape = physics.createShape(geom, *material);
    m_actor = physics.createRigidStatic(PxTransform);
    m_actor->attachShape(*shape);
    shape->release();
}

void xrPhysX::CDB::MODEL::AddPrototype(const xr_span<const Fvector3>& vertices, const xr_span<const ::CDB::TRI>& faces)
{
    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.data = vertices.data();
    meshDesc.points.count = vertices.size();
    meshDesc.points.stride = sizeof(Fvector3);
    meshDesc.triangles.data = faces.data();
    meshDesc.triangles.count = faces.size();
    meshDesc.triangles.stride = sizeof(::CDB::TRI);

    physx::PxTolerancesScale scale;
    physx::PxCookingParams params(scale);

#ifdef DEBUG
    PxValidateTriangleMesh(params, meshDesc);
#endif
    
    m_prototypes.emplace_back(
        PxCreateTriangleMesh(params, meshDesc,
        PhysXInstance::GetInstance().GetPhysics().getPhysicsInsertionCallback())
    );
    auto& Prototype = m_prototypes.back();
    Prototype.SetExtraData(faces);
}

physx::PxAgain xrPhysX::CDB::xrRaycastBuffer::processTouches(const physx::PxRaycastHit* buffer, physx::PxU32 nbHits)
{
    for (physx::PxU32 i = 0; i < nbHits; i++)
    {
        const auto& hit = buffer[i];

        if ((bool)(options&TraceOptions::cull) && !IsFrontFace(hit))
        {
            continue;
        }

        if ((bool)(options&TraceOptions::full_test) && !IsValidTriangleHit(hit))
        {
            continue;
        }

        ProcessValidHit(hit);

        if ((bool)(options&TraceOptions::only_first))
        {
            block = hit;
            return false;
        }
    }
    
    return true;
}

bool xrPhysX::CDB::xrRaycastBuffer::IsFrontFace(const physx::PxRaycastHit& hit) const
{
    if (!IVERIFY(hit.shape && hit.faceIndex == physx::PxU32(-1)))
    {
        return false;
    }

    physx::PxVec3 Normal = GetTriangleNormal(hit.shape, hit.faceIndex);

    return hit.normal.dot(Normal) < 0.0f;
}

void xrPhysX::CDB::MODEL::AddInstances(u32 prototype, const xr_vector<xr_pair<Fmatrix, u16>>& instances)
{
    m_instances.reserve(m_instances.size() + instances.size());
    for (const auto& instance : instances)
    {
        m_instances.emplace_back(this, instance.first, instance.second, prototype);
    }
}

xrPhysX::CDB::MODEL::MODEL()
{
    auto& Physics = PhysXInstance::GetInstance().GetPhysics();

    physx::PxSceneDesc SceneDescr(Physics.getTolerancesScale());
    SceneDescr.gravity = physx::PxVec3(0, -9.8f, 0); // TODO: read gravity from configs
    SceneDescr.cpuDispatcher = PhysXInstance::GetInstance().GetDefaultCpuDispatcher();
	SceneDescr.filterShader	= physx::PxDefaultSimulationFilterShader;
    m_scene = Physics.createScene(SceneDescr);
}

xrPhysX::CDB::MODEL::~MODEL()
{
    m_scene->release();
}

const xrPhysX::CDB::CollisionPrototype& xrPhysX::CDB::MODEL::GetPrototype(u32 ID)
{
    return m_prototypes[ID];
}

void xrPhysX::CDB::MODEL::AddUniqueStaticGeom(const xr_span<const Fvector3>& vertices,
                                              const xr_span<const ::CDB::TRI>& faces)
{
    AddPrototype(vertices, faces);
    AddInstances(m_prototypes.size()-1, {{Fmatrix().identity(), SectorInvalid}});
}

void xrPhysX::CDB::MODEL::AddInstances(const xr_span<const Fvector3>& vertices, const xr_span<const ::CDB::TRI>& faces,
    const xr_vector<xr_pair<Fmatrix, u16>>& Instances)
{
    AddPrototype(vertices, faces);
    AddInstances(m_prototypes.size()-1, Instances);
}

void xrPhysX::CDB::MODEL::Finalize()
{
    for (const auto& instance : m_instances)
    {
        m_scene->addActor(instance.GetActor());
    }
    m_scene->flushSimulation();
}

void xrPhysX::CDB::MODEL::RayTrace(const RayTraceOptions& options, RayTraceResult& result)
{
    physx::PxRaycastBuffer buffer;
    physx::PxHitFlags flags = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;;
    if(!(bool)(options.options & TraceOptions::cull))
    {
        flags |= physx::PxHitFlag::eMESH_BOTH_SIDES;
    }
    if((bool)(options.options & TraceOptions::full_test))
    {
        flags |= physx::PxHitFlag::eFACE_INDEX;
        flags |= physx::PxHitFlag::ePRECISE_SWEEP;
    }
    if((bool)(options.options & TraceOptions::only_first))
    {
        flags |= physx::PxHitFlag::eANY_HIT;
    }
    /*else
    {
        flags |= physx::PxHitFlag::eNO_BLOCK;
    }*/
    if (m_scene->raycast(options.GetStart(), options.GetDir(), options.r_range, buffer))
    {
        
    }
}
