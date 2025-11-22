#pragma once
#include "PxArticulationLink.h"
#include "PxArticulationReducedCoordinate.h"
#include "PxPhysXConfig.h"
#include "PhysX/Interfaces/ShellHolder.h"

struct SJointIKData;
class IBoneData;

namespace xrPhysX
{
    bool XRCORE_API can_create_phys_shell(string1024& reason, Interfaces::IShellHolder& O);
}

namespace xrPhysX::Wrappers
{
    using namespace physx;
    class XRCORE_API CArticulation
    {
        const IKinematics* m_kinematics = nullptr;
        PxArticulationReducedCoordinate* m_articulation = nullptr;
        PxArticulationLink* m_root = nullptr;
        xr_vector<PxArticulationLink*> m_links = {};
        float m_totalMass = 0.0f;

#ifdef DEBUG
        const Interfaces::IShellHolder* m_dbg_obj = nullptr;
#endif

        // Create
        void build_FromKinematics(const IKinematics& kinematics);
        void AddElementRecursive(PxArticulationLink* parent, u16 id, Fmatrix global_parent);
        void ConfigureLinkProperties(PxArticulationLink& Link, const IBoneData& BoneData);
        void CreateArticulationJoint(PxArticulationLink& Parent, PxArticulationLink& Child, const IBoneData& BoneData, u16 ID);
        void ConfigureJointFrames(PxArticulationJointReducedCoordinate& joint, PxArticulationLink& Parent, PxArticulationLink& Child, const IBoneData& BoneData);
        
        void SetupRigitJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData);
        void SetupRevoluteJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData);
        void SetupPrismaticJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData);
        void SetupSphericalJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData);
        void SetupWheelJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData);

        void SetupJointDamping(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData);
        void SetupJointFriction(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData);
        void SetupBreakableJoint(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData);
        void SetupJointDrives(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData);
        void SetupJointLimits(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData);

        //Release
        void ReleaseArticulationLinks(PxArticulationLink& link);

        // Influence
        PxVec3 CalculateForceFromAcceleration(const Fvector& acceleration);
        void ApplyForceToAllLinks(const PxVec3& force);
        void ApplyForceRecursive(PxArticulationLink& link, const PxVec3& force);
        
    public:
        CArticulation(const Interfaces::IShellHolder& holder);

        void Deactivate();

        void InterpolateGlobalTransform(Fmatrix& m) const;
	    void applyAccel(const Fvector& accel);
    };
}
