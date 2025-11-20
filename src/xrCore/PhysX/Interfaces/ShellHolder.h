#pragma once

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

        virtual const Wrappers::CShell& GetShell() const = 0;
        virtual Wrappers::CShell& GetShell() = 0;
    };
}
