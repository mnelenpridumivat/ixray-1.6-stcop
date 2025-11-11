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

xrPhysX::CDB::CollisionInstance::CollisionInstance(const Fmatrix& transform, const physx::PxTriangleMesh* prototype)
{
    m_shared_mesh = const_cast<physx::PxTriangleMesh*>(prototype);
    physx::PxTransform PxTransform = PhysXMathHelper::Conv_MatrixToPxTransform(transform);
    physx::PxVec3 pxScale = PhysXMathHelper::Conv_MatrixToPxScale(transform);

    physx::PxTriangleMeshGeometry geom(m_shared_mesh);
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
    
    m_prototypes.emplace_back(PxCreateTriangleMesh(params, meshDesc,
        PhysXInstance::GetInstance().GetPhysics().getPhysicsInsertionCallback()));
}

void xrPhysX::CDB::MODEL::AddInstances(physx::PxTriangleMesh* prototype, const xr_vector<Fmatrix>& instances)
{
    m_instances.reserve(m_instances.size() + instances.size());
    for (const auto& instance : instances)
    {
        m_instances.emplace_back(instance, prototype);
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

void xrPhysX::CDB::MODEL::Finalize()
{
    for (const auto& instance : m_instances)
    {
        m_scene->addActor(instance.GetActor());
    }
    m_scene->flushSimulation();
}
