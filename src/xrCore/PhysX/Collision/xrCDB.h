#pragma once
#include <PxRigidStatic.h>
#include "_stl_extensions.h"
#include "pvd/PxPvd.h"

namespace CDB
{
    class TRI;
}

namespace xrPhysX::CDB
{
    class CollisionInstance {
        physx::PxRigidStatic* m_actor;
        physx::PxTriangleMesh* m_shared_mesh;
    
    public:
        CollisionInstance(const Fmatrix& transform, 
                         const physx::PxTriangleMesh* prototype);

        physx::PxRigidStatic& GetActor() const {return *m_actor;}
    };
    
    class MODEL
    {
        xr_vector<physx::PxTriangleMesh*> m_prototypes;
        xr_vector<CollisionInstance> m_instances;
        physx::PxScene* m_scene;

        void AddInstances(physx::PxTriangleMesh* prototype, const xr_vector<Fmatrix>& instances);
    public:
        MODEL();
        ~MODEL();

        void AddPrototype(const xr_span<const Fvector3>& vertices, const xr_span<const ::CDB::TRI>& faces);

        void Finalize();
        
        
    };
}
