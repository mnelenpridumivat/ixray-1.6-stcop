#pragma once
#include "customzone.h"
#include "../xrScripts/script_export_space.h"

class CEmiZone :
	public CCustomZone
{
private:
	typedef		CCustomZone					inherited;

public:
	void	Load(LPCSTR section) override;

	void	enter_Zone(SZoneObjectInfo& io) override;
	void	exit_Zone(SZoneObjectInfo& io) override;

	DECLARE_SCRIPT_REGISTER_FUNCTION

};

