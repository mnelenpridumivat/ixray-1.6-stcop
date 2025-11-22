#include "stdafx.h"
#include "Articulation.h"

#include <magic_enum/magic_enum.hpp>

#include "PxArticulationJointReducedCoordinate.h"
#include "PxArticulationLink.h"
#include "PxScene.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xrEngine/bone.h"
#include "extensions/PxRigidBodyExt.h"
#include "PhysX/PhysXCore.h"

constexpr u32 MaxChildrenNum = 64;

using namespace physx;
using namespace xrPhysX;
using namespace xrPhysX::Wrappers;

namespace xrPhysX
{
    ICF bool no_physics_shape(const SBoneShape& shape)
    {
        return shape.type==SBoneShape::stNone||shape.flags.test(SBoneShape::sfNoPhysics);
    }

    bool ShapeIsPhysic(const SBoneShape& shape)
    {
        return !no_physics_shape( shape );
    }

    bool BoneHasPhysics(const IKinematics& kinematics, u16 bone_id)
    {
        return kinematics.LL_GetBoneVisible( bone_id ) && ShapeIsPhysic(kinematics.GetBoneData(bone_id).get_shape());
    }

    bool VerifyPhysicsCollisionShapes(const IKinematics& kinematics)
    {
        u16 nbb = kinematics.LL_BoneCount();
        for(u16 i = 0; i < nbb; ++i )
        {
            if(BoneHasPhysics(kinematics, i))
            {
                return true;
            }
        }
        return false;
    }

    IC bool check_obb_sise(const Fobb& obb)
    {
        return (!fis_zero(obb.m_halfsize.x,EPS_L)||
            !fis_zero(obb.m_halfsize.y,EPS_L)||
            !fis_zero(obb.m_halfsize.z,EPS_L));
    }

    bool can_create_phys_shell(string1024& reason, Interfaces::IShellHolder& O)
    {
        xr_strcpy(reason, "ok" );
        bool result = true;
        IKinematics* K =O.ObjectKinematics();
        if(!K)
        {
            xr_strcpy(reason, make_string<const char*>( "Can not create physics shell for object %s, model %s is not skeleton", O.ObjectName(), O.ObjectNameVisual() ));
            return false;
        }
        if(!VerifyPhysicsCollisionShapes(*K))
        {
            xr_strcpy(reason, make_string<const char*>( "Can not create physics shell for object %s, model %s has no physics collision shapes set", O.ObjectName(), O.ObjectNameVisual() ));
            return false;
        }
        if(!_valid( O.ObjectXFORM() ))
        {
            xr_strcpy( reason, make_string<const char*>( "create physics shell: object matrix is not valid" ));
            return false;
        }
        /*if(!valid_pos( O.ObjectXFORM().c ))
        {
#ifdef	DEBUG
            xr_strcpy( reason, dbg_valide_pos_string( O.ObjectXFORM().c, &O, "create physics shell" ).c_str() );
#else
            xr_strcpy(reason, make_string<const char*>("~ create physics shell: object position is not valid, or missing 'level.cform', or the level is not loaded."));
#endif
            return false;
        }*/
        return result;
    }
}

void CArticulation::build_FromKinematics(const IKinematics& kinematics)
{
    m_kinematics = &kinematics;
    AddElementRecursive(nullptr, kinematics.LL_GetBoneRoot(), Fidentity);
}

void CArticulation::AddElementRecursive(PxArticulationLink* parent, u16 id, Fmatrix global_parent)
{
    const IBoneData& BoneData = m_kinematics->GetBoneData(id);
    Fmatrix BoneTransform = m_kinematics->LL_GetTransform(id);

    PxTransform physxBoneTransform = PhysXMathHelper::Conv_MatrixToPxTransform(BoneTransform);

    auto Link = m_articulation->createLink(parent, physxBoneTransform);
    VERIFY(Link);
    if (!m_root && !parent)
    {
        m_root = Link;
    }

    ConfigureLinkProperties(*Link, BoneData);

    if (parent)
    {
        CreateArticulationJoint(*parent, *Link, BoneData, id);
    }

    for (u16 i = 0; i < BoneData.GetNumChildren(); ++i)
    {
        AddElementRecursive(Link, BoneData.GetChild(i).GetSelfID(), BoneTransform);
    }
    
    Link->setMass(BoneData.get_mass());
    m_totalMass += BoneData.get_mass();
}

void CArticulation::ConfigureLinkProperties(PxArticulationLink& Link, const IBoneData& BoneData)
{
    auto& obb = BoneData.get_obb();
    PxBoxGeometry BoxGeom(obb.m_halfsize.x, obb.m_halfsize.y, obb.m_halfsize.z);
    PxTransform LocalPos = PhysXMathHelper::Conv_PosAndRotToPxTransform({}, obb.m_rotate);

    PxShape* Shape = PhysXInstance::GetPhysicsStatic().createShape(BoxGeom, PhysXMaterialManager::GetInstance().GetDefaultMaterial(), true);
    Shape->setLocalPose(LocalPos);
    Link.attachShape(*Shape);
    Shape->release();

    PxRigidBodyExt::setMassAndUpdateInertia(Link, BoneData.get_mass());

    PxVec3 MassCenter;
    {
        auto& xrMassCenter = BoneData.get_center_of_mass();
        MassCenter.x = xrMassCenter.x;
        MassCenter.y = xrMassCenter.y;
        MassCenter.z = xrMassCenter.z;
    }
    Link.setCMassLocalPose(PxTransform(MassCenter));
}

void CArticulation::CreateArticulationJoint(PxArticulationLink& Parent, PxArticulationLink& Child, const IBoneData& BoneData,
    u16 ID)
{
    auto joint = Child.getInboundJoint();
    VERIFY(joint);

    ConfigureJointFrames(*joint, Parent, Child, BoneData);

    auto& ikData = BoneData.get_IK_data();
    switch (ikData.type)
    {
    case jtNone:
        {
            SetupSphericalJoint(*joint, BoneData);
            break;
        }
    case jtRigid:
        {
            SetupRigitJoint(*joint, BoneData);
            break;
        }
    case jtCloth:
        {
            FATAL("Not implemented");
            break;
        }
    case jtJoint:
        {
            SetupRevoluteJoint(*joint, BoneData);
            break;
        }
    case jtWheel:
        {
            SetupWheelJoint(*joint, BoneData);
            break;
        }
    case jtSlider:
        {
            SetupPrismaticJoint(*joint, BoneData);
            break;
        }
    default:
        {
            xr_string str("IK type [");
            str+=magic_enum::enum_name<EJointType>(ikData.type);
            str+="] not valid!";
            FATAL(str.c_str());
        }
    }
    SetupJointDamping(*joint, ikData);
    SetupJointFriction(*joint, ikData);
    SetupJointDrives(*joint, ikData);
    SetupJointLimits(*joint, ikData);
    if (ikData.ik_flags.is(SJointIKData::flBreakable))
    {
        SetupBreakableJoint(*joint, ikData);
    }
}

void CArticulation::ConfigureJointFrames(PxArticulationJointReducedCoordinate& joint, PxArticulationLink& Parent,
    PxArticulationLink& Child, const IBoneData& BoneData)
{
    auto& bindTransform = BoneData.get_bind_transform();

    PxTransform parentFrame = PhysXMathHelper::Conv_MatrixToPxTransform(bindTransform);
    PxTransform childFrame = PxTransform(PxIdentity);

    joint.setParentPose(parentFrame);
    joint.setChildPose(childFrame);

    /*
    if (!boneData.IK_data.anchor_offset.similar(Fvector3().set(0,0,0)))
    {
        Fvector anchor = boneData.IK_data.anchor_offset;
        physx::PxVec3 pxAnchor(anchor.x, anchor.y, anchor.z);
        joint->setChildPose(physx::PxTransform(pxAnchor));
    }*/
}

void CArticulation::SetupRigitJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData)
{
    joint.setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eX, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eY, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eZ, PxArticulationMotion::eLOCKED);
}

void CArticulation::SetupRevoluteJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData)
{
    joint.setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eFREE);
    joint.setMotion(PxArticulationAxis::eX, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eY, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eZ, PxArticulationMotion::eLOCKED);
}

void CArticulation::SetupPrismaticJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData)
{
    joint.setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eX, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eY, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eZ, PxArticulationMotion::eFREE);
}

void CArticulation::SetupSphericalJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData)
{
    joint.setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eFREE);
    joint.setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eFREE);
    joint.setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eFREE);
    joint.setMotion(PxArticulationAxis::eX, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eY, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eZ, PxArticulationMotion::eLOCKED);
}

void CArticulation::SetupWheelJoint(PxArticulationJointReducedCoordinate& joint, const IBoneData& BoneData)
{
    joint.setMotion(PxArticulationAxis::eSWING1, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eTWIST, PxArticulationMotion::eFREE);
    joint.setMotion(PxArticulationAxis::eX, PxArticulationMotion::eLOCKED);
    joint.setMotion(PxArticulationAxis::eY, PxArticulationMotion::eFREE);
    joint.setMotion(PxArticulationAxis::eZ, PxArticulationMotion::eLOCKED);
}

void CArticulation::SetupJointDamping(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData)
{
    float damping = ikData.damping_factor * 0.1f;
    float stiffness = ikData.spring_factor * 10.0f;
    
    for (PxArticulationAxis::Enum axis = PxArticulationAxis::eX; 
         axis <= PxArticulationAxis::eTWIST; 
         axis = PxArticulationAxis::Enum(axis + 1))
    {
        if (joint.getMotion(axis) == physx::PxArticulationMotion::eFREE)
        {
            PxArticulationDrive drive;
            drive.damping = damping;
            drive.stiffness = stiffness;
            joint.setDriveParams(axis, drive);
        }
    }
}

void CArticulation::SetupJointFriction(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData)
{
    float friction = ikData.friction;
    
    for (PxArticulationAxis::Enum axis = PxArticulationAxis::eX; 
         axis <= PxArticulationAxis::eTWIST; 
         axis = PxArticulationAxis::Enum(axis + 1))
    {
        if (joint.getMotion(axis) == PxArticulationMotion::eFREE)
        {
            PxJointFrictionParams params;
            params.viscousFrictionCoefficient = friction;
            joint.setFrictionParams(axis, params);
        }
    }
}

void CArticulation::SetupBreakableJoint(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData)
{
    // TODO: implement
}

void CArticulation::SetupJointDrives(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData)
{
    /*if (!ikData.motor_active)
    {
        return;
    }

    // Настройка моторов для joint'а
    for (int i = 0; i < 3; ++i) // Для трех осей
    {
        if (ikData.motor_limits[i].active)
        {
            physx::PxArticulationAxis::Enum axis = GetPhysXAxisFromIndex(i);
            
            if (joint->getMotion(axis) == physx::PxArticulationMotion::eFREE)
            {
                // Устанавливаем привод с заданной силой/скоростью
                float forceLimit = ikData.motor_limits[i].force;
                float velocityTarget = ikData.motor_limits[i].velocity;
                
                joint->setDriveTarget(axis, velocityTarget);
                joint->setDriveVelocity(axis, velocityTarget);
                joint->setMaxForce(axis, forceLimit);
                
                // Настраиваем ПИД-регулятор привода
                joint->setDrive(axis, ikData.motor_limits[i].KP, 
                                      ikData.motor_limits[i].KD, 
                                      0.0f);
            }
        }
    }*/
}

void CArticulation::SetupJointLimits(PxArticulationJointReducedCoordinate& joint, const SJointIKData& ikData)
{
    PxArticulationLimit limit;
    limit.low = ikData.limits[0].limit.x;
    limit.high = ikData.limits[0].limit.y;
    joint.setLimitParams(PxArticulationAxis::eSWING1, limit);
    limit.low = ikData.limits[1].limit.x;
    limit.high = ikData.limits[1].limit.y;
    joint.setLimitParams(PxArticulationAxis::eSWING2, limit);
    limit.low = ikData.limits[2].limit.x;
    limit.high = ikData.limits[2].limit.y;
    joint.setLimitParams(PxArticulationAxis::eTWIST, limit);
}

void CArticulation::ReleaseArticulationLinks(PxArticulationLink& link)
{
    {
        auto ChildrenNum = link.getNbChildren();
        VERIFY(ChildrenNum <= MaxChildrenNum);
        xr_array<PxArticulationLink*, MaxChildrenNum> Children;
        link.getChildren(Children.data(), MaxChildrenNum);
        for (auto child : Children)
        {
            ReleaseArticulationLinks(*child);
        }
    }

    auto Shapes = link.getNbShapes();
    VERIFY(Shapes <= MaxChildrenNum);
    xr_array<PxShape*, MaxChildrenNum> Children;
    link.getShapes(Children.data(), MaxChildrenNum);

    for (auto shape : Children)
    {
        link.detachShape(*shape);
        shape->release();
    }
    
}

PxVec3 CArticulation::CalculateForceFromAcceleration(const Fvector& acceleration)
{
    PxVec3 pxAcceleration(acceleration.x, acceleration.y, acceleration.z);
    return pxAcceleration * m_totalMass;
}

void CArticulation::ApplyForceToAllLinks(const PxVec3& force)
{
    if (m_root)
    {
        ApplyForceRecursive(*m_root, force);
    }
}

void CArticulation::ApplyForceRecursive(PxArticulationLink& link, const PxVec3& force)
{
    link.addForce(force);

    xr_array<PxArticulationLink*, MaxChildrenNum> Children;
    link.getChildren(Children.data(), MaxChildrenNum);
    for (auto child : Children)
    {
        ApplyForceRecursive(*child, force);
    }
}

CArticulation::CArticulation(const Interfaces::IShellHolder& holder)
{
    m_articulation = PhysXInstance::GetPhysicsStatic().createArticulationReducedCoordinate();
    VERIFY(m_articulation);
    auto Kinematics = holder.ObjectKinematics();
    IVERIFY_M(Kinematics, "Can not create physics shell for object %s, model %s is not skeleton", holder.ObjectName(), holder.ObjectNameVisual());
    auto& KinematicsRef = *Kinematics;
    IVERIFY_M(VerifyPhysicsCollisionShapes(KinematicsRef), "Can not create physics shell for object %s, model %s has no physics collision shapes set", holder.ObjectName(), holder.ObjectNameVisual());
    IVERIFY_M(_valid(holder.ObjectXFORM()), "create physics shell: object matrix is not valid");
    
#ifdef DEBUG
    m_dbg_obj = &holder;
#endif
    build_FromKinematics(KinematicsRef);
}

void CArticulation::Deactivate()
{
    auto Scene = m_articulation->getScene();
    VERIFY(Scene);
    Scene->removeArticulation(*m_articulation);
    ReleaseArticulationLinks(*m_root);
    xr_delete(m_articulation);
}

void CArticulation::InterpolateGlobalTransform(Fmatrix& m) const
{
    auto root = m_articulation->getRootGlobalPose();
    PhysXMathHelper::Conv_PxTransformToMatrix(m, root);
}

void CArticulation::applyAccel(const Fvector& accel)
{
    PxVec3 totalForce = CalculateForceFromAcceleration(accel);
    ApplyForceToAllLinks(totalForce);
}
