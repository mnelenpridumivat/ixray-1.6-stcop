#pragma once
#include "ExplosiveRocket.h"

class CMissileSam final: public CExplosiveRocket
{
	using inherited = CExplosiveRocket;

	CGameObject* target = nullptr;

public:
	void SetTarget(CGameObject* target) { this->target = target; }
	virtual	void			StartFlying() override;

	virtual	void			UpdateEnginePh() override;

protected:

	virtual bool IsEngineInfinite() override {return true;}

};
