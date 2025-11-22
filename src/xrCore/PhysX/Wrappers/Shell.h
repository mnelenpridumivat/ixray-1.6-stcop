#pragma once

#include "Base.h"
#include "JointType.h"
#include "../Interfaces/ShellHolder.h"

struct SBoneShape;
class IKinematics;

namespace xrPhysX::Wrappers
{
    class XRCORE_API CShell;
    class XRCORE_API CJoint;
    class XRCORE_API CElement;
    
    using shell_root = std::pair<CShell*,u16>;

    struct physicsBone
    {
        CJoint* joint = nullptr;
        CElement* element = nullptr;
    };

    class XRCORE_API CShell : public CBase
    {
        physx::PxArticulationReducedCoordinate* m_articulation = nullptr;
        xr_vector<CElement*> m_elements{};
        xr_vector<CJoint*> m_joints{};
        IKinematics* m_kinematics = nullptr;
        Interfaces::IShellHolder* m_owner = nullptr;

        float m_LinearDamping = 0.f;
        float m_AngularDamping = 0.f;
        bool m_active = false;
        bool m_animated = false;

#ifdef DEBUG
        const Interfaces::IShellHolder* m_dbg_obj = nullptr;
#endif
        
        CShell() = default;

        void build_FromKinematics(IKinematics& kinematics);
        void AddElementRecursive(CElement* root_e, u16 id, Fmatrix global_parent, u16 element_number, bool* vis_check);

    public:        
        static CShell* Create(const Interfaces::IShellHolder& holder, bool not_active_state);
        /*CPhysXShell(physx::PxScene* scene, IPhysXShellHolder* owner);
        virtual ~CPhysXShell();
        
        void Activate(const Fmatrix& startTransform, bool disable = false);
        void Activate(const Fmatrix& transform, const Fvector& linVel, const Fvector& angVel, bool disable = false);*/
        void Deactivate();
        /*bool IsActive() const;
        
        void BuildFromKinematics(IKinematics* kinematics, xr_map<u16, physicsBone>* boneMap = nullptr);
        void BuildSimpleShell(float mass, bool notActiveState);
        
        CPhysXElement* CreateElement();
        CPhysXJoint* CreateJoint(JointType type, 
                                CPhysXElement* elem1, CPhysXElement* elem2,
                                const Fvector& anchor);
    
        CPhysXElement* GetElement(u16 boneID);
        CPhysXElement* GetElement(const shared_str& boneName);
        u16 GetElementsNumber() const;
        
        void SetLinearVelocity(const Fvector& vel);
        void SetAngularVelocity(const Fvector& vel);
        void ApplyForce(const Fvector& force);
        void ApplyImpulse(const Fvector& impulse);
        void ApplyCentralImpulse(const Fvector& impulse);
        
        void SetCollisionGroup(u16 group);
        void EnableCollision();
        void DisableCollision();
        void SetIgnoreStatic();
        void SetIgnoreDynamic();
        
        void SetMass(float mass);
        void SetDensity(float density);
        void UpdateMassProperties();
        
        void SetAnimated(bool animated);
        void InterpolateGlobalTransform(Fmatrix* matrix);
        void UpdateFromAnimation(bool calculateBones = true);
        void UpdateToAnimation();
        
        void SetupBreaking(float breakForce, float breakTorque);
        bool IsFractured() const;
        void SplitProcess(xr_vector<shell_root>& outShells);
        
        void Net_Import(NET_Packet& packet);
        void Net_Export(NET_Packet& packet);
        
#ifdef DEBUG
        void RenderDebug(float scale, u32 color);
#endif
        
        physx::PxArticulationReducedCoordinate* GetPhysXArticulation() const { return m_articulation; }
        void AddToScene(physx::PxScene* scene);
        void RemoveFromScene(physx::PxScene* scene);*/
    };
}
