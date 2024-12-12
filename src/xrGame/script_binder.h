////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script objects binder
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Save/SaveObject.h"

class CSE_Abstract;
class CScriptBinderObject;
class NET_Packet;
class CSaveObject;
class CGameObject;

class CScriptBinder {
protected:
	CScriptBinderObject			*m_object;
	CGameObject* m_Owner;

public:
								CScriptBinder	(CGameObject* Owner);
	virtual						~CScriptBinder	();
			void				init			();
			void				clear			();
	virtual void				reinit			();
	virtual void				Load			(LPCSTR section);
	virtual void				reload			(LPCSTR section);
	virtual BOOL				net_Spawn		(CSE_Abstract* DC);
	virtual void				net_Destroy		();
	virtual void				shedule_Update	(u32 time_delta);
	virtual void				save			(NET_Packet &output_packet);
	virtual void				load			(IReader &input_packet);
	virtual void Save(CSaveObjectSave* Object);
	virtual void Load(CSaveObjectLoad* Object);
	virtual BOOL				net_SaveRelevant();
	virtual void				net_Relcase		(CObject *object);
			void				set_object		(CScriptBinderObject *object);
	IC		CScriptBinderObject	*object			();
};

#include "script_binder_inline.h"