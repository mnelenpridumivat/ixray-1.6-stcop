#pragma once
#include "Base.h"

namespace xrPhysX::Wrappers
{
    class XRCORE_API CShell;

    class XRCORE_API CElement : public CBase
    {
        physx::PxRigidActor* m_actor = nullptr;
        CShell* m_shell = nullptr;
        u16 m_SelfID = u16(-1);
        xr_vector<physx::PxShape*> m_Shapes;

        void CreateShape(const physx::PxGeometry& geom, const physx::PxTransform& transform, float density);

        void UpdateDensity(float density);

    public:
        CElement(physx::PxScene* scene);
        virtual ~CElement();

        void AddSphere(const Fsphere& sphere, float density = 1.0f);
        void AddBox(const Fobb& box, float density = 1.0f);
        void AddCapsule(const Fcylinder& cylinder, float density = 1.0f);
        //void AddConvexMesh(const xr_span<const Fvector> vertices, float density = 1.0f);
        
        void SetMass(float mass);
        void SetDensity(float density);
        //void SetMassSpaceInertiaTensor(const Fvector& inertia);
        
        //void SetLinearVelocity(const Fvector& vel);
        //void SetAngularVelocity(const Fvector& vel);
        //void AddForce(const Fvector& force);
        //void AddTorque(const Fvector& torque);
        
        //void SetCollisionGroup(u16 group);
        //void SetMaterial(u16 material_idx);
    
        physx::PxRigidActor* GetPhysXActor() const { return m_actor; }
    };
}