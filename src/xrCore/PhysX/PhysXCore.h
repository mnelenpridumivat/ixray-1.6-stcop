#pragma once
#include "PxPhysics.h"
#include "foundation/PxAllocator.h"
#include "foundation/PxErrorCallback.h"
#include "foundation/PxFoundation.h"
#include "memory/xrMemory_subst_msvc.h"
#include "pvd/PxPvd.h"
#include "extensions/PxDefaultCpuDispatcher.h"

namespace xrPhysX
{
    physx::PxQuat Conv_MatrixToPxQuat(const Fmatrix& transform);
    void Conv_MatrixToPxQuat(physx::PxQuat& target, const Fmatrix& transform);
    
    physx::PxQuat Conv_MatrixToPxQuatNoScale(const Fmatrix& transform);
    void Conv_MatrixToPxQuatNoScale(physx::PxQuat& target, const Fmatrix& transform);
    
    physx::PxVec3 Conv_MatrixToPxScale(const Fmatrix& transform);
    void Conv_MatrixToPxScale(physx::PxVec3& target, const Fmatrix& transform);

    physx::PxTransform Conv_MatrixToPxTransform(const Fmatrix& transform);
    void Conv_MatrixToPxTransform(physx::PxTransform& target, const Fmatrix& transform);

    physx::PxTransform Conv_MatrixToPxTransformNoScale(const Fmatrix& transform);
    void Conv_MatrixToPxTransformNoScale(physx::PxTransform& target, const Fmatrix& transform);
    
    class xrPhysXAllocator : public physx::PxAllocatorCallback
    {
    public:
        virtual void* allocate(size_t size, const char* typeName, const char* filename, int line);
        virtual void deallocate(void* ptr);
    };

    class xrPhysXErrorCallback : public physx::PxErrorCallback
    {
    public:
        virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
    };

    class PhysXMaterialManager
    {
        // TODO: will ne storage for multiple materials
        xr_hash_map<shared_str, physx::PxMaterial*> materials;
        
        PhysXMaterialManager();
    public:
        ~PhysXMaterialManager();

        PhysXMaterialManager(const PhysXMaterialManager& other) = delete;
        PhysXMaterialManager& operator=(const PhysXMaterialManager& other) = delete;
        PhysXMaterialManager(PhysXMaterialManager&& other) = delete;
        PhysXMaterialManager& operator=(PhysXMaterialManager&& other) = delete;

        static PhysXMaterialManager& GetInstance();
        
        physx::PxMaterial* GetMaterial(const shared_str& materialName);
        physx::PxMaterial* GetDefaultMaterial();
    };
    
    class PhysXInstance
    {
        xr_unique_ptr<xrPhysXAllocator> Allocator;
        xr_unique_ptr<xrPhysXErrorCallback> ErrorCallback;
        physx::PxFoundation* Foundation = nullptr;
        physx::PxPhysics* Physics = nullptr;
        physx::PxDefaultCpuDispatcher* Dispatcher = nullptr;
#ifdef DEBUG
        physx::PxPvd* PVD;
#endif
        
        PhysXInstance();
    public:
        PhysXInstance(const PhysXInstance& rhs) = delete;
        PhysXInstance(PhysXInstance&& rhs) = delete;
        PhysXInstance& operator=(const PhysXInstance& rhs) = delete;
        PhysXInstance& operator=(PhysXInstance&& rhs) = delete;

        ~PhysXInstance();

        physx::PxPhysics& GetPhysics();
        physx::PxDefaultCpuDispatcher* GetDefaultCpuDispatcher();

        static PhysXInstance& GetInstance();
    };
}
