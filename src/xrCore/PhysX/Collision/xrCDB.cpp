#include "stdafx.h"
#include "xrCDB.h"
#include "Collision/xrCDB.h"

#include "CFormBuilder.h"
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

xrPhysX::CDB::CollisionInstance::CollisionInstance(MODEL* model, const Fmatrix& transform, u16 sector, u32 prototype) noexcept
{
    //static_assert(std::is_nothrow_constructible_v<xrPhysX::CDB::CollisionInstance>);
    //static_assert(std::is_nothrow_assignable_v<CollisionInstance, CollisionInstance>);
    
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
    m_actor->userData = this; // no move or copy = no need for stable ref
    //stable_ref = xr_make_unique<StableInstanceRef>(CollisionModel, CollisionModel->GetInstancesNum());
    //m_actor->userData = stable_ref.get(); // if we just put a pointer to this, after vector reallocation in MODEL pointer in userData will become invalid
    shape->release();
}

/*xrPhysX::CDB::CollisionInstance::CollisionInstance(CollisionInstance&& other) noexcept
{
    CollisionModel = other.CollisionModel;
    m_actor = other.m_actor;
    stable_ref = std::move(other.stable_ref);
    m_shared_mesh = other.m_shared_mesh;
    Sector = other.Sector;
    other.CollisionModel = nullptr;
    other.m_actor = nullptr;
    other.m_shared_mesh = u32(-1);
    other.Sector = SectorInvalid;
}

xrPhysX::CDB::CollisionInstance& xrPhysX::CDB::CollisionInstance::operator=(CollisionInstance&& other) noexcept
{
    CollisionModel = other.CollisionModel;
    m_actor = other.m_actor;
    stable_ref = std::move(other.stable_ref);
    m_shared_mesh = other.m_shared_mesh;
    Sector = other.Sector;
    other.CollisionModel = nullptr;
    other.m_actor = nullptr;
    other.m_shared_mesh = u32(-1);
    other.Sector = SectorInvalid;
    return *this;
}*/

/*u32 xrPhysX::CDB::CollisionInstance::GetID() const
{
    return stable_ref->GetID();
}*/

const xrPhysX::CDB::CollisionPrototype& xrPhysX::CDB::CollisionInstance::GetPrototype() const
{
    return CollisionModel->GetPrototype(m_shared_mesh);
}

/*const xrPhysX::CDB::CollisionInstance& xrPhysX::CDB::StableInstanceRef::GetCollisionInstance() const
{
    return CollisionModel->GetInstance(ID);
}*/

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
    
    m_prototypes.emplace_back(xr_make_unique<CollisionPrototype>(
        PxCreateTriangleMesh(params, meshDesc,
        PhysXInstance::GetInstance().GetPhysics().getPhysicsInsertionCallback())
    ));
    auto& Prototype = m_prototypes.back();
    Prototype->SetExtraData(faces);
}

void xrPhysX::CDB::MODEL::ConvertHitToVertices(const physx::PxRaycastHit& hit, Fvector vertices[3])
{
    auto shape = hit.shape;
    physx::PxGeometryHolder geom = shape->getGeometry();
    VERIFY(geom.getType() == physx::PxGeometryType::eTRIANGLEMESH);
    auto& mesh_geom = geom.triangleMesh();
    auto mesh = mesh_geom.triangleMesh;
    const physx::PxU32* indices = (const physx::PxU32*)(mesh->getTriangles());
    const physx::PxVec3* positions = mesh->getVertices();
        
    VERIFY(indices && positions);

    physx::PxTransform globalTransform = hit.actor->getGlobalPose();
    physx::PxU32 baseIndex = hit.faceIndex * 3;
        
    for (int i = 0; i < 3; ++i) {
        physx::PxVec3 localVertex = positions[indices[baseIndex + i]];
            
        if (mesh_geom.scale.isValidForTriangleMesh()) {
            localVertex = localVertex.multiply(mesh_geom.scale.scale);
        }
            
        physx::PxVec3 globalVertex = globalTransform.transform(localVertex);
            
        vertices[i].x = globalVertex.x;
        vertices[i].y = globalVertex.y;
        vertices[i].z = globalVertex.z;
    }
    
    
}

void xrPhysX::CDB::MODEL::ConvertHitToResult(const physx::PxRaycastHit& hit, ::CDB::RESULT& result)
{
    ConvertHitToVertices(hit, result.verts);
    
    result.u = hit.u;
    result.v = hit.v;
    result.range = hit.distance;
    result.id = hit.faceIndex;

    CollisionInstance* Instance = (CollisionInstance*)(hit.actor->userData);
    auto& Prototype = Instance->GetPrototype();
    auto& Triangle = Prototype.GetTriangeData(result.id);
    
    result.data.material = Triangle.material;
    result.data.suppress_shadows = Triangle.suppress_shadows;
    result.data.suppress_wm = Triangle.suppress_wm;
    auto RealSector = Instance->GetSector();
    if (RealSector == SectorInvalid)
    {
        RealSector = Triangle.sector;
    }
    result.data.sector = RealSector;
}

/*void xrPhysX::CDB::MODEL::ConvertHitToResult(const physx::PxOverlapHit& hit, TraceOptions options, xr_vector<::CDB::RESULT>& result)
{
    VERIFY(hit.actor && hit.shape);

    auto shape = hit.shape;
    physx::PxGeometryHolder geom = shape->getGeometry();
    VERIFY(geom.getType() == physx::PxGeometryType::eTRIANGLEMESH);
    auto& mesh_geom = geom.triangleMesh();
    auto mesh = mesh_geom.triangleMesh;
    
    if ((bool)(options&TraceOptions::only_nearest))
    {
        GetIntersectingTriangles(hit, mesh, options, result);
    } else
    {
        
    }
    
    ::CDB::RESULT res;
    res.id = ;
    res.verts[0] = ;
    res.verts[1] = ;
    res.verts[2] = ;
    res.data.dummy = ;
}*/

/*void xrPhysX::CDB::MODEL::GetIntersectingTriangles(const physx::PxOverlapHit& hit,
    const physx::PxTriangleMesh* geom, TraceOptions options, xr_vector<::CDB::RESULT>& result)
{
    const physx::PxU32* indices = (const physx::PxU32*)(geom->getTriangles());
    const physx::PxVec3* vertices = geom->getVertices();
    physx::PxU32 triangleCount = geom->getNbTriangles();

    VERIFY(indices && vertices);

    auto transform = hit.actor->getGlobalPose();

    for (physx::PxU32 i = 0; i < triangleCount; ++i)
    {
        Fvector verts[3];
        if (
            GetTriangleGlobalVertices(vertices, indices, i, transform, geom, verts)
            && IsTriangleIntersecting(verts, )
            )
        {
            
        }
    }
    
}*/

void xrPhysX::CDB::MODEL::ConvertHitsToResults(xr_span<physx::PxOverlapHit> hits, TraceOptions options,
                                               xr_vector<::CDB::RESULT>& result)
{
    for (const auto& hit : hits)
    {
        FATAL("Not implemented");
        if ((bool)(options&TraceOptions::full_test))
        {
            //ConvertHitToResult(hit, options, result);
        } else
        {
            //result.emplace_back();
            //ConvertHitToResult(hit, options, result);
        }
        
        if ((bool)(options&TraceOptions::only_nearest))
        {
            break;
        }
    }
}

void xrPhysX::CDB::MODEL::ExecuteBoxTrace(const physx::PxBoxGeometry& geom, const physx::PxTransform& transform,
                                          TraceOptions options, physx::PxQueryFlags QueryFlags, TraceResult& result)
{
    physx::PxOverlapBuffer buffer;
    bool hasAny = m_scene->overlap(geom, transform, buffer, 
                                  physx::PxQueryFilterData(QueryFlags));
    if (hasAny)
    {
        VERIFY(buffer.hasAnyHits());
        if ((bool)(options&TraceOptions::only_first))
        {
            ConvertHitsToResults({buffer.touches, 1}, options, result.results);
        } else if ((bool)(options&TraceOptions::only_nearest))
        {
            ConvertHitsToResults({buffer.touches, buffer.nbTouches}, options, result.results);
        }
    }
}

/*physx::PxAgain xrPhysX::CDB::xrRaycastBuffer::processTouches(const physx::PxRaycastHit* buffer, physx::PxU32 nbHits)
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
}*/

void xrPhysX::CDB::MODEL::AddInstances(u32 prototype, const xr_vector<CformInstance::InstanceData>& instances)
{
    m_instances.reserve(m_instances.size() + instances.size());
    for (const auto& instance : instances)
    {
        m_instances.emplace_back(xr_make_unique<CollisionInstance>(this, instance.transform, instance.sector, prototype));
        //VERIFY(m_instances.back()->GetID() == m_instances.size()-1);
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
    VERIFY(m_prototypes.size() > ID);
    return *m_prototypes[ID];
}

const xrPhysX::CDB::CollisionInstance& xrPhysX::CDB::MODEL::GetInstance(u32 ID)
{
    VERIFY(m_instances.size() > ID);
    return *m_instances[ID];
}

void xrPhysX::CDB::MODEL::AddUniqueStaticGeom(const xr_span<const Fvector3>& vertices,
                                              const xr_span<const ::CDB::TRI>& faces)
{
    AddPrototype(vertices, faces);
    AddInstances(m_prototypes.size()-1, {{Fmatrix().identity(), SectorInvalid}});
}

void xrPhysX::CDB::MODEL::AddInstances(const xr_span<const Fvector3>& vertices, const xr_span<const ::CDB::TRI>& faces,
    const xr_vector<CformInstance::InstanceData>& Instances)
{
    AddPrototype(vertices, faces);
    AddInstances(m_prototypes.size()-1, Instances);
}

void xrPhysX::CDB::MODEL::Finalize()
{
    for (const auto& instance : m_instances)
    {
        m_scene->addActor(instance->GetActor());
    }
    m_scene->flushSimulation();
    ready = true;
}

void xrPhysX::CDB::MODEL::RayTrace(const RayTraceOptions& options, TraceResult& result)
{
    VERIFY(ready);
    physx::PxHitFlags HitFlags = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
    physx::PxQueryFlags QueryFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC;
    if(!(bool)(options.options & TraceOptions::cull))
    {
        HitFlags |= physx::PxHitFlag::eMESH_BOTH_SIDES;
    }
    if((bool)(options.options & TraceOptions::full_test))
    {
        HitFlags |= physx::PxHitFlag::eFACE_INDEX;
        HitFlags |= physx::PxHitFlag::ePRECISE_SWEEP;
    }
    if((bool)(options.options & TraceOptions::only_first))
    {
        QueryFlags |= physx::PxQueryFlag::eANY_HIT;
    }
    else
    {
        QueryFlags |= physx::PxQueryFlag::eNO_BLOCK;
    }

    xr_vector<physx::PxRaycastHit> hits;

    if (QueryFlags & physx::PxQueryFlag::eANY_HIT)
    {
        physx::PxRaycastBuffer buffer;
        bool hasHit = m_scene->raycast(
            options.GetStart(),
            options.GetDir(),
            options.r_range,
            buffer,
            HitFlags,
            physx::PxQueryFilterData(QueryFlags));
        
        if (hasHit)
        {
            hits.push_back(buffer.block);
        }
    } else
    {
        
        physx::PxRaycastBuffer buffer;
        bool hasHits = m_scene->raycast(
            options.GetStart(),
            options.GetDir(),
            options.r_range,
            buffer,
            HitFlags,
            physx::PxQueryFilterData(QueryFlags));

        if (hasHits)
        {
            if ((bool)(options.options & TraceOptions::only_nearest))
            {
                VERIFY(buffer.nbTouches > 0);
                hits.push_back(buffer.touches[0]);
            } else
            {
                hits.reserve(buffer.nbTouches);
                for (physx::PxU32 i = 0; i < buffer.nbTouches; ++i)
                {
                    hits.push_back(buffer.touches[i]);
                }
                VERIFY(buffer.hasBlock && buffer.block.position == buffer.touches[0].position);
            }
        }
    }

    for (auto& hit : hits)
    {
        auto& res = result.results.emplace_back();
        ConvertHitToResult(hit, res);
    }
}

void xrPhysX::CDB::MODEL::BoxTrace(const AABBBoxTraceOptions& options, TraceResult& result)
{
    VERIFY(ready);
    physx::PxQueryFlags QueryFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eNO_BLOCK;
    if ((bool)(options.options & TraceOptions::only_first))
    {
        QueryFlags |= physx::PxQueryFlag::eANY_HIT;
    }

    physx::PxBoxGeometry geom(options.GetExtents());
    physx::PxTransform post(options.GetCenter());

    ExecuteBoxTrace(geom, post, options.options, QueryFlags, result);
}

void xrPhysX::CDB::MODEL::BoxTrace(const OBBBoxTraceOptions& options, TraceResult& result)
{
    VERIFY(ready);
    physx::PxQueryFlags QueryFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eNO_BLOCK;
    if ((bool)(options.options & TraceOptions::only_first))
    {
        QueryFlags |= physx::PxQueryFlag::eANY_HIT;
    }

    physx::PxBoxGeometry geom(options.GetExtents());
    physx::PxTransform post(options.GetCenter(), options.GetRot());
    
    ExecuteBoxTrace(geom, post, options.options, QueryFlags, result);
}
