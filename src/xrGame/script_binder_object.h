////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrScripts/script_export_space.h"
#include "Save/SaveInterface.h"

class CSE_ALifeObject;
class CScriptGameObject;
class NET_Packet;
class CSaveObject;

class CScriptBinderObject {
public:
	typedef CSE_ALifeObject*	SpawnType;
	CScriptGameObject			*m_object;

public:
						CScriptBinderObject	(CScriptGameObject *object);
	virtual				~CScriptBinderObject();
	virtual void		reinit				();
	virtual void		reload				(LPCSTR section);
	virtual bool		net_Spawn			(SpawnType DC);
	virtual void		net_Destroy			();
	virtual void		net_Import			(NET_Packet *net_packet);
	virtual void		net_Export			(NET_Packet *net_packet);
	virtual void		shedule_Update		(u32 time_delta);
	virtual void		save				(NET_Packet *output_packet);
	virtual void		load				(IReader	*input_packet);
	//virtual void Save(ISaveObject* Object);
	//virtual void Load(ISaveObject* Object);
	virtual void Serialize(ISaveObject& Object);
	virtual bool		net_SaveRelevant	();
	virtual void		net_Relcase			(CScriptGameObject *object);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};