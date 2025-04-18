#pragma once

namespace Logic
{
    class CTransition
    {
    public:
        virtual void OnTransitionActivate() = 0;
        virtual void OnTransitionDeactivate() = 0;

        virtual bool CheckConditions() = 0;
    };
}
