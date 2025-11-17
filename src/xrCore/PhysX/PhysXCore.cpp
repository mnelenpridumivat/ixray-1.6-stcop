#include "stdafx.h"
#include "PhysXCore.h"

#include "PxMaterial.h"
#include "common/PxTolerancesScale.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "foundation/PxPhysicsVersion.h"
#include "memory/xrMemory.h"
#include "pvd/PxPvdTransport.h"

physx::PxQuat xrPhysX::PhysXMathHelper::Conv_MatrixToPxQuat(const Fmatrix& transform)
{
    physx::PxQuat quat;
    Conv_MatrixToPxQuat(quat, transform);
    return quat;
}

void xrPhysX::PhysXMathHelper::Conv_MatrixToPxQuat(physx::PxQuat& target, const Fmatrix& transform)
{
    Fmatrix m = transform;
    physx::PxVec3 PxScale;
    Conv_MatrixToPxScale(PxScale, m);
    if (PxScale.x > 0)
    {
        m._11 /= PxScale.x; m._12 /= PxScale.x; m._13 /= PxScale.x;
    }
    if (PxScale.y > 0)
    {
        m._21 /= PxScale.y; m._22 /= PxScale.y; m._23 /= PxScale.y;
    }
    if (PxScale.z > 0)
    {
        m._31 /= PxScale.z; m._32 /= PxScale.z; m._33 /= PxScale.z;
    }
    Conv_MatrixToPxQuatNoScale(target, m);
}

physx::PxQuat xrPhysX::PhysXMathHelper::Conv_MatrixToPxQuat(const Fmatrix33& transform)
{
    physx::PxQuat quat;
    Conv_MatrixToPxQuat(quat, transform);
    return quat;
}

void xrPhysX::PhysXMathHelper::Conv_MatrixToPxQuat(physx::PxQuat& target, const Fmatrix33& transform)
{
    Fmatrix33 m = transform;
    physx::PxVec3 PxScale;
    Conv_MatrixToPxScale(PxScale, m);
    if (PxScale.x > 0)
    {
        m._11 /= PxScale.x; m._12 /= PxScale.x; m._13 /= PxScale.x;
    }
    if (PxScale.y > 0)
    {
        m._21 /= PxScale.y; m._22 /= PxScale.y; m._23 /= PxScale.y;
    }
    if (PxScale.z > 0)
    {
        m._31 /= PxScale.z; m._32 /= PxScale.z; m._33 /= PxScale.z;
    }
    Conv_MatrixToPxQuatNoScale(target, m);
}

physx::PxQuat xrPhysX::PhysXMathHelper::Conv_MatrixToPxQuatNoScale(const Fmatrix& transform)
{
    physx::PxQuat quat;
    VERIFY(fabs(transform.i.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.j.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.k.magnitude() - 1) < EPS);
    Conv_MatrixToPxQuatNoScale(quat, transform);
    return quat;
}

void xrPhysX::PhysXMathHelper::Conv_MatrixToPxQuatNoScale(physx::PxQuat& target, const Fmatrix& transform)
{
    VERIFY(fabs(transform.i.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.j.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.k.magnitude() - 1) < EPS);
    // Упрощенная конвертация матрицы 3x3 в кватернион
    float trace = transform._11 + transform._22 + transform._33;
    
    if (trace > 0) {
        float s = 0.5f / physx::PxSqrt(trace + 1.0f);
        target.x = (transform._32 - transform._23) * s;
        target.y = (transform._13 - transform._31) * s;
        target.z = (transform._21 - transform._12) * s;
        target.w = 0.25f / s;
    } else if (transform._11 > transform._22 && transform._11 > transform._33) {
        float s = 2.0f * physx::PxSqrt(1.0f + transform._11 - transform._22 - transform._33);
        target.x = 0.25f * s;
        target.y = (transform._12 + transform._21) / s;
        target.z = (transform._13 + transform._31) / s;
        target.w = (transform._32 - transform._23) / s;
    } else if (transform._22 > transform._33) {
        float s = 2.0f * physx::PxSqrt(1.0f + transform._22 - transform._11 - transform._33);
        target.x = (transform._12 + transform._21) / s;
        target.y = 0.25f * s;
        target.z = (transform._23 + transform._32) / s;
        target.w = (transform._13 - transform._31) / s;
    } else {
        float s = 2.0f * physx::PxSqrt(1.0f + transform._33 - transform._11 - transform._22);
        target.x = (transform._13 + transform._31) / s;
        target.y = (transform._23 + transform._32) / s;
        target.z = 0.25f * s;
        target.w = (transform._21 - transform._12) / s;
    }
}

physx::PxQuat xrPhysX::PhysXMathHelper::Conv_MatrixToPxQuatNoScale(const Fmatrix33& transform)
{
    physx::PxQuat quat;
    VERIFY(fabs(transform.i.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.j.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.k.magnitude() - 1) < EPS);
    Conv_MatrixToPxQuatNoScale(quat, transform);
    return quat;
}

void xrPhysX::PhysXMathHelper::Conv_MatrixToPxQuatNoScale(physx::PxQuat& target, const Fmatrix33& transform)
{
    VERIFY(fabs(transform.i.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.j.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.k.magnitude() - 1) < EPS);
    // Упрощенная конвертация матрицы 3x3 в кватернион
    float trace = transform._11 + transform._22 + transform._33;
    
    if (trace > 0) {
        float s = 0.5f / physx::PxSqrt(trace + 1.0f);
        target.x = (transform._32 - transform._23) * s;
        target.y = (transform._13 - transform._31) * s;
        target.z = (transform._21 - transform._12) * s;
        target.w = 0.25f / s;
    } else if (transform._11 > transform._22 && transform._11 > transform._33) {
        float s = 2.0f * physx::PxSqrt(1.0f + transform._11 - transform._22 - transform._33);
        target.x = 0.25f * s;
        target.y = (transform._12 + transform._21) / s;
        target.z = (transform._13 + transform._31) / s;
        target.w = (transform._32 - transform._23) / s;
    } else if (transform._22 > transform._33) {
        float s = 2.0f * physx::PxSqrt(1.0f + transform._22 - transform._11 - transform._33);
        target.x = (transform._12 + transform._21) / s;
        target.y = 0.25f * s;
        target.z = (transform._23 + transform._32) / s;
        target.w = (transform._13 - transform._31) / s;
    } else {
        float s = 2.0f * physx::PxSqrt(1.0f + transform._33 - transform._11 - transform._22);
        target.x = (transform._13 + transform._31) / s;
        target.y = (transform._23 + transform._32) / s;
        target.z = 0.25f * s;
        target.w = (transform._21 - transform._12) / s;
    }
}

physx::PxVec3 xrPhysX::PhysXMathHelper::Conv_MatrixToPxScale(const Fmatrix& transform)
{
    physx::PxVec3 pxScale;
    Conv_MatrixToPxScale(pxScale, transform);
    return pxScale;
}

void xrPhysX::PhysXMathHelper::Conv_MatrixToPxScale(physx::PxVec3& target, const Fmatrix& transform)
{
    target.x = transform.i.magnitude();
    target.y = transform.j.magnitude();
    target.z = transform.k.magnitude();
}

physx::PxVec3 xrPhysX::PhysXMathHelper::Conv_MatrixToPxScale(const Fmatrix33& transform)
{
    physx::PxVec3 pxScale;
    Conv_MatrixToPxScale(pxScale, transform);
    return pxScale;
}

void xrPhysX::PhysXMathHelper::Conv_MatrixToPxScale(physx::PxVec3& target, const Fmatrix33& transform)
{
    target.x = transform.i.magnitude();
    target.y = transform.j.magnitude();
    target.z = transform.k.magnitude();
}

physx::PxTransform xrPhysX::PhysXMathHelper::Conv_MatrixToPxTransform(const Fmatrix& transform)
{
    physx::PxTransform target;
    Conv_MatrixToPxTransform(target, transform);
    return target;
}

void xrPhysX::PhysXMathHelper::Conv_MatrixToPxTransform(physx::PxTransform& target, const Fmatrix& transform)
{
    target.p.x = transform.c.x; target.p.y = transform.c.y; target.p.z = transform.c.z;
    Conv_MatrixToPxQuat(target.q, transform);
}

physx::PxTransform xrPhysX::PhysXMathHelper::Conv_MatrixToPxTransformNoScale(const Fmatrix& transform)
{
    VERIFY(fabs(transform.i.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.j.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.k.magnitude() - 1) < EPS);
    physx::PxTransform target;
    Conv_MatrixToPxTransformNoScale(target, transform);
    return target;
}

void xrPhysX::PhysXMathHelper::Conv_MatrixToPxTransformNoScale(physx::PxTransform& target, const Fmatrix& transform)
{
    VERIFY(fabs(transform.i.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.j.magnitude() - 1) < EPS);
    VERIFY(fabs(transform.k.magnitude() - 1) < EPS);
    target.p.x = transform.c.x; target.p.y = transform.c.y; target.p.z = transform.c.z;
    Conv_MatrixToPxQuatNoScale(target.q, transform);
}

physx::PxTransform xrPhysX::PhysXMathHelper::Conv_PosToPxTransform(const Fvector& pos)
{
    physx::PxTransform target;
    Conv_PosToPxTransform(target, pos);
    return target;
}

void xrPhysX::PhysXMathHelper::Conv_PosToPxTransform(physx::PxTransform& target, const Fvector& pos)
{
    target.p.x = pos.x;
    target.p.y = pos.y;
    target.p.z = pos.z;
}

physx::PxTransform xrPhysX::PhysXMathHelper::Conv_PosAndRotToPxTransform(const Fvector& pos, const Fmatrix33& rot)
{
    physx::PxTransform target;
    Conv_PosAndRotToPxTransform(target, pos, rot);
    return target;
}

void xrPhysX::PhysXMathHelper::Conv_PosAndRotToPxTransform(physx::PxTransform& target, const Fvector& pos,
    const Fmatrix33& rot)
{
    target.p.x = pos.x;
    target.p.y = pos.y;
    target.p.z = pos.z;
    Conv_MatrixToPxQuat(target.q, rot);
}

physx::PxTransform xrPhysX::PhysXMathHelper::Conv_PosAndRotToPxTransformNoScale(const Fvector& pos, const Fmatrix33& rot)
{
    physx::PxTransform target;
    Conv_PosAndRotToPxTransformNoScale(target, pos, rot);
    return target;
}

void xrPhysX::PhysXMathHelper::Conv_PosAndRotToPxTransformNoScale(physx::PxTransform& target, const Fvector& pos,
    const Fmatrix33& rot)
{
    target.p.x = pos.x;
    target.p.y = pos.y;
    target.p.z = pos.z;
    Conv_MatrixToPxQuatNoScale(target.q, rot);
}

void* xrPhysX::xrPhysXAllocator::allocate(size_t size, const char* typeName, const char* filename, int line)
{
    void* Ptr = xr_alloc(size);
    if (!I_ASSERT_M((size_t(Ptr) & 15)==0, "Unable to allocate memory with required alignment (current %d)", (size_t(Ptr) & 15)))
    {
        xr_free(Ptr);
        return nullptr;
    }
#if PX_STOMP_ALLOCATED_MEMORY
    if(ptr != NULL)
    {
        PxMemSet(ptr, PxI32(0xcd), PxU32(size));
    }
#endif
    return Ptr;
}

void xrPhysX::xrPhysXAllocator::deallocate(void* ptr)
{
    xr_free(ptr);
}

void xrPhysX::xrPhysXErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file,
    int line)
{
    using namespace physx;
    const char* errorCode = NULL;

    switch (code)
    {
    case PxErrorCode::eNO_ERROR:
        {
            errorCode = "no error";
            break;
        }
    case PxErrorCode::eINVALID_PARAMETER:
        {
            errorCode = "invalid parameter";
            break;
        }
    case PxErrorCode::eINVALID_OPERATION:
        {
            errorCode = "invalid operation";
            break;
        }
    case PxErrorCode::eOUT_OF_MEMORY:
        {
            errorCode = "out of memory";
            break;
        }
    case PxErrorCode::eDEBUG_INFO:
        {
            errorCode = "info";
            break;
        }
    case PxErrorCode::eDEBUG_WARNING:
        {
            errorCode = "warning";
            break;
        }
    case PxErrorCode::ePERF_WARNING:
        {
            errorCode = "performance warning";
            break;
        }
    case PxErrorCode::eABORT:
        {
            errorCode = "abort";
            break;
        }
    case PxErrorCode::eINTERNAL_ERROR:
        {
            errorCode = "internal error";
            break;
        }
    case PxErrorCode::eMASK_ALL:
        {
            errorCode = "unknown error";
            break;
        }
    }

    I_ASSERT_M(false, "PhysX error: %s. File %s. Line %d. Message %s", errorCode, file, line, message);

    if (code == PxErrorCode::eABORT)
    {
        FATAL("PhysX abort");
    }
    
}

xrPhysX::PhysXMaterialManager::PhysXMaterialManager()
{
    auto& physics = PhysXInstance::GetInstance().GetPhysics();
    // Базовый материал для статической геометрии уровня
    materials["default_static"] = physics.createMaterial(0.5f, 0.4f, 0.1f);
        
    // Для разных типов поверхностей
    materials["metal"] = physics.createMaterial(0.7f, 0.5f, 0.1f);
    materials["wood"] = physics.createMaterial(0.4f, 0.3f, 0.2f);
    materials["stone"] = physics.createMaterial(0.6f, 0.5f, 0.05f);
    materials["ice"] = physics.createMaterial(0.1f, 0.05f, 0.01f);
    
}

xrPhysX::PhysXMaterialManager::~PhysXMaterialManager()
{
    for (auto& material : materials)
    {
        material.second->release();
    }
}

xrPhysX::PhysXMaterialManager& xrPhysX::PhysXMaterialManager::GetInstance()
{
    static PhysXMaterialManager instance;
    return instance;
}

physx::PxMaterial* xrPhysX::PhysXMaterialManager::GetMaterial(const shared_str& materialName)
{
    auto it = materials.find(materialName);
    if (!I_ASSERT_M(it != materials.end(), "Physics data for material [%s] not found! Fallback to default.", materialName.c_str()))
    {
        return GetDefaultMaterial();
    }
    return it->second;
}

physx::PxMaterial* xrPhysX::PhysXMaterialManager::GetDefaultMaterial()
{
    VERIFY(false, "Direct GetDefaultMaterial call!");
    return materials["default_static"];
}

xrPhysX::PhysXInstance::PhysXInstance()
{
    Allocator = xr_make_unique<xrPhysXAllocator>();
    ErrorCallback = xr_make_unique<xrPhysXErrorCallback>();
    Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *Allocator, *ErrorCallback);

#ifdef DEBUG
    PVD = physx::PxCreatePvd(*Foundation);
    auto transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    PVD->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif

    Physics = PxCreatePhysics(
        PX_PHYSICS_VERSION, *Foundation, physx::PxTolerancesScale(), true,
#ifdef DEBUG
        PVD
#else
        nullptr
#endif
        );
    
    Dispatcher = physx::PxDefaultCpuDispatcherCreate(1); // TODO: pass multiple threads num
}

xrPhysX::PhysXInstance::~PhysXInstance()
{
    Physics->release();
#ifdef DEBUG
    {
        auto transport = PVD->getTransport();
        transport->release();
        PVD->release();
    }
#endif
    Foundation->release();
    Dispatcher->release();
}

physx::PxPhysics& xrPhysX::PhysXInstance::GetPhysics()
{
    VERIFY(Physics);
    return *Physics;
}

physx::PxDefaultCpuDispatcher* xrPhysX::PhysXInstance::GetDefaultCpuDispatcher()
{
    VERIFY(Dispatcher);
    return Dispatcher;
}

xrPhysX::PhysXInstance& xrPhysX::PhysXInstance::GetInstance()
{
    static PhysXInstance instance;
    return instance;
}

physx::PxPhysics& xrPhysX::PhysXInstance::GetPhysicsStatic()
{
    return GetInstance().GetPhysics();
}
