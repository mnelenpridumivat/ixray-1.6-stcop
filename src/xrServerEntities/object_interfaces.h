////////////////////////////////////////////////////////////////////////////
//	Module 		: object_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife interfaces
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Save/SaveObject.h"
#include "FS.h"

class NET_Packet;

class IPureDestroyableObject {
public:
	virtual void					destroy()											= 0;
};

template <typename _storage_type>
class IPureLoadableObject {
public:
	virtual void					load(_storage_type	&storage)						= 0;
};

template <typename _storage_type>
class IPureSavableObject {
public:
	virtual void					save(_storage_type	&storage)						= 0;
};

template <typename _storage_type_load, typename _storage_type_save>
class IPureSerializeObject : public IPureLoadableObject<_storage_type_load>, public IPureSavableObject<_storage_type_save> {
public:
};

class IPureStateUpdateObject {
public:
	virtual void					STATE_Write(NET_Packet& tNetPacket) = 0;
	virtual void					STATE_Read(NET_Packet& tNetPacket, u16 size) = 0;
	virtual void					UPDATE_Write(NET_Packet& tNetPacket) = 0;
	virtual void					UPDATE_Read(NET_Packet& tNetPacket) = 0;
	//virtual void					STATE_WriteSave(CSaveObjectSave* Object) const = 0;
	//virtual void					STATE_ReadSave(CSaveObjectLoad* Object) = 0;
	virtual void					STATE_Serialize(ISaveObject& Object) = 0;
	//virtual void					UPDATE_WriteSave(CSaveObjectSave* Object) const = 0;
	//virtual void					UPDATE_ReadSave(CSaveObjectLoad* Object) = 0;
	virtual void					UPDATE_Serialize(ISaveObject& Object) = 0;
};

class IPureServerObject : public IPureSerializeObject<IReader,IWriter>, public IPureStateUpdateObject {};

class IPureSchedulableObject {
public:
	virtual void					update() = 0;
};
