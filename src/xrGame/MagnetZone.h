#pragma once
#include "customzone.h"
#include "../xrScripts/script_export_space.h"

class CMagnetZone :
    public CCustomZone
{
private:
	typedef		CCustomZone					inherited;

protected:
	//���� �������� ���������� � ���� (��� ���� 100 ��)
	float			m_fThrowInImpulse;
	//���� �������� ���������� � ���� ��� ����� �������
	float			m_fThrowInImpulseAlive;
	//�����. ����������� (��� ������, ��� ������� ����������)
	float			m_fThrowInAtten;

public:
	void	Load(LPCSTR section) override;

	//����������� ����� �� ������
	void	Affect(SZoneObjectInfo* O) override;
	virtual void	AffectPull(CPhysicsShellHolder* GO, const Fvector& throw_in_dir, float dist);
	virtual void	AffectPullAlife(CEntityAlive* EA, const Fvector& throw_in_dir, float dist);
	virtual void	AffectPullDead(CPhysicsShellHolder* GO, const Fvector& throw_in_dir, float dist);
	virtual void	ThrowInCenter(Fvector& C);

	bool	BlowoutState() override;

	DECLARE_SCRIPT_REGISTER_FUNCTION

};

