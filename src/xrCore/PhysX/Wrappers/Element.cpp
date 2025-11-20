#include "stdafx.h"
#include "Element.h"

#include <algorithm>

#include "PxRigidActor.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "extensions/PxRigidBodyExt.h"
#include "PhysX/PhysXCore.h"

using namespace physx;
using namespace xrPhysX;

Wrappers::CElement::CElement(PxScene* scene) : CBase()
{
    m_scene = scene;
}

Wrappers::CElement::~CElement()
{
    if (!m_Shapes.empty())
    {
        VERIFY(m_actor);
        for (auto shape : m_Shapes)
        {
            if (shape)
            {
                m_actor->detachShape(*shape);
                shape->release();
            }
        }
        m_Shapes.clear();
    }

    if (m_actor)
    {
        auto Scene = m_actor->getScene();
        if (Scene)
        {
            Scene->removeActor(*m_actor);
        }

        m_actor->release();
        m_actor = nullptr;
    }
    
}

void Wrappers::CElement::CreateShape(const physx::PxGeometry& geom, const physx::PxTransform& transform,
    float density)
{
    if (!IVERIFY(m_actor))
    {
        return;
    }

    auto& Physics = PhysXInstance::GetPhysicsStatic();
    auto& MaterialManager = PhysXMaterialManager::GetInstance();
    auto material = MaterialManager.GetDefaultMaterial(); // TODO: add actual material get

    auto NewShape = Physics.createShape(geom, *material, true);
    if (IVERIFY(NewShape))
    {
        NewShape->setLocalPose(transform);

        FATAL("Finish this!");
        // TODO: Uncomment and fix
        /*auto FilterData = GetFilterData();
        NewShape->setSimulationFilterData(FilterData);
        NewShape->setQueryFilterData(FilterData);*/

        m_actor->attachShape(*NewShape);
        m_Shapes.push_back(NewShape);

        if (density > 0.0f && m_actor->is<PxRigidDynamic>())
        {
            PxRigidDynamic* ptr = (PxRigidDynamic*)m_actor;
            PxRigidBodyExt::updateMassAndInertia(*ptr, density);
        }
        
    }
}

void Wrappers::CElement::UpdateDensity(float density)
{
    if (IVERIFY(density > 0.0f) && IVERIFY(m_actor) && IVERIFY(m_actor->is<PxRigidDynamic>()))
    {
        auto Actor = (PxRigidDynamic*)m_actor;

        /* If something go wrong - try uncomment this
        // Detach all shapes and reattach with density
        for (PxShape* shape : m_Shapes)
        {
            Actor->detachShape(*shape);
        }
        
        // Reattach shapes with density
        for (PxShape* shape : m_Shapes)
        {
            Actor->attachShape(*shape);
        }*/

        PxRigidBodyExt::updateMassAndInertia(*Actor, density);
    }
}

void Wrappers::CElement::AddSphere(const Fsphere& sphere, float density)
{
    PxSphereGeometry geom(sphere.R);
    PxTransform pos = PhysXMathHelper::Conv_PosToPxTransform(sphere.P);
    CreateShape(geom, pos, density);
}

void Wrappers::CElement::AddBox(const Fobb& box, float density)
{
    PxBoxGeometry geom(box.m_halfsize.x, box.m_halfsize.y, box.m_halfsize.z);

    PxTransform pose = PhysXMathHelper::Conv_PosAndRotToPxTransformNoScale(box.m_translate, box.m_rotate);
    CreateShape(geom, pose, density);
}

void Wrappers::CElement::AddCapsule(const Fcylinder& cylinder, float density)
{
    float halfHeight = cylinder.m_height * 0.5f - cylinder.m_radius;
    halfHeight = std::max(halfHeight, 0.0f);

    PxCapsuleGeometry geom(cylinder.m_radius, halfHeight);
    
    Fmatrix transform;
    transform.identity();
    Fmatrix rotation;
    rotation.identity();
    transform.mulA_43(rotation);
    transform.c.set(cylinder.m_center);
    
    PxTransform pose = PhysXMathHelper::Conv_MatrixToPxTransformNoScale(transform);
    CreateShape(geom, pose, density);
}

void Wrappers::CElement::SetMass(float mass)
{
    if (IVERIFY(mass >= 0) && IVERIFY(m_actor) && m_actor->is<PxRigidDynamic>())
    {
        auto Actor = (PxRigidDynamic*)m_actor;
        PxRigidBodyExt::setMassAndUpdateInertia(*Actor, mass);
    }
}

void Wrappers::CElement::SetDensity(float density)
{
    UpdateDensity(density);
}
