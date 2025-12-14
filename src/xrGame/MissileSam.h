#pragma once
#include "ExplosiveRocket.h"

class CMissileSam final: public CExplosiveRocket
{
	using inherited = CExplosiveRocket;

	CGameObject* target = nullptr;

public:
	CMissileSam() : CExplosiveRocket(){ m_affect_gravity = false; }
	void SetTarget(CGameObject* target) { this->target = target; }

	virtual	void			UpdateEnginePh() override;

protected:

	virtual bool IsEngineInfinite() override {return true;}

};
