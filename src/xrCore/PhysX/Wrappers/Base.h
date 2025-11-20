#pragma once
#include "PxScene.h"

namespace xrPhysX::Wrappers
{
    class XRCORE_API CBase
    {
    protected:
        physx::PxScene* m_scene = nullptr;
        Fmatrix m_XFORM{};
        
    public:
        virtual ~CBase() = default;
        virtual void SetTransform(const Fmatrix& xform) = 0;
        virtual const Fmatrix& GetTransform() const { return m_XFORM; }
        virtual void SetUserData(void* data) = 0;
    };
}
