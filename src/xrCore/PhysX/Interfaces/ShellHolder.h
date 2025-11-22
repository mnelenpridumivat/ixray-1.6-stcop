#pragma once

class IKinematics;

namespace xrPhysX::Wrappers
{
    class CShell;
}

namespace xrPhysX::Interfaces
{
    class XRCORE_API IShellHolder
    {
    public:
        virtual ~IShellHolder() = default;

        //virtual const Wrappers::CShell& GetShell() const = 0;
        //virtual Wrappers::CShell& GetShell() = 0;
        
        virtual	const IKinematics* ObjectKinematics() const = 0;
        virtual	IKinematics* ObjectKinematics() = 0;
        virtual	const Fmatrix& ObjectXFORM() const = 0;
        virtual	LPCSTR ObjectName() const = 0;
        virtual	LPCSTR ObjectNameVisual() const = 0;
    };
}
