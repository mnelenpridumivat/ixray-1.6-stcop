#pragma once
#include "customzone.h"
#include "../xrScripts/script_export_space.h"

class CMagnetZone :
    public CCustomZone
{
private:
	typedef		CCustomZone					inherited;

protected:
	//сила импульса вт€гивани€ в зону (дл€ веса 100 кг)
	float			m_fThrowInImpulse;
	//сила импульса вт€гивани€ в зону дл€ живых существ
	float			m_fThrowInImpulseAlive;
	//коэфф. зат€гивани€ (чем меньше, тем плавнее зат€гивает)
	float			m_fThrowInAtten;

public:
	void	Load(LPCSTR section) override;

	//воздействие зоной на объект
	void	Affect(SZoneObjectInfo* O) override;
	virtual void	AffectPull(CPhysicsShellHolder* GO, const Fvector& throw_in_dir, float dist);
	virtual void	AffectPullAlife(CEntityAlive* EA, const Fvector& throw_in_dir, float dist);
	virtual void	AffectPullDead(CPhysicsShellHolder* GO, const Fvector& throw_in_dir, float dist);
	virtual void	ThrowInCenter(Fvector& C);

	bool	BlowoutState() override;

	DECLARE_SCRIPT_REGISTER_FUNCTION

};

