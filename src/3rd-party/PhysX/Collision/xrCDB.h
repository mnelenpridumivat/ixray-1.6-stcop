#pragma once
#include <PxRigidStatic.h>

namespace xrPhysX::CDB
{
    class CollisionInstance {
    private:
        physx::PxRigidStatic* m_actor;
        physx::PxTriangleMesh* m_shared_mesh;
        std::string m_mesh_id;
    
    public:
        CollisionInstance(const physx::PxTransform& transform, 
                         const std::string& mesh_id);
        void SetTransform(const physx::PxTransform& transform);
        // ... другие методы
    };
    
    class MODEL
    {
        
    };
}
