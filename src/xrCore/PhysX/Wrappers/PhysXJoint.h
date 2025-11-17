#pragma once
#include "extensions/PxJoint.h"
#include "PhysXJointType.h"

namespace xrPhysX::Wrappers
{    
    class XRCORE_API CPhysXElement;

    class XRCORE_API CPhysXJoint
    {
        physx::PxJoint* m_joint = nullptr;
        CPhysXElement* m_element1 = nullptr;
        CPhysXElement* m_element2 = nullptr;

    public:

        /*CPhysXJoint(JointType type, CPhysXElement* element1, CPhysXElement* element2,
            const Fvector& anchor, const Fvector& axis = Fvector{0, 1, 0});

        void SetLimits(float low, float high, int axis = 0);
        void SetStiffness(float stiffness, float damping);
        void SetBreakForce(float force, float torque);
        void SetDriveVelocity(float velocity, int axis = 0);
        void SetDriveForceLimit(float force, int axis = 0);

        bool IsBroken() const;*/
    };
}
