////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_object_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife object registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_ALife.h"
#include "Save/SaveObject.h"

class CALifeObjectRegistry
{
public:
	typedef xr_map<ALife::_OBJECT_ID, CSE_ALifeDynamicObject*>	OBJECT_REGISTRY;

protected:
	OBJECT_REGISTRY					m_objects;

	// todo: see search manager, random access iterator must be for using clipper, sadly maps are not usable for this due to linear complexity (and so clipper is useless in such containers)
	// todo: needed to be refactored because Level contains objects as vector so it is better to have vector and some unordered_set for searching id if we want to make searching faster than std::find of vector?
	xr_vector<CSE_ALifeDynamicObject*> m_objects_as_vec;
private:
			void					save					(IWriter &memory_stream, CSE_ALifeDynamicObject *object, u32 &object_count);
			//void					Save(CSaveObjectSave* Object, CSE_ALifeDynamicObject* object, u32& object_count) const;
			void					Serialize(ISaveObject& Object, CSE_ALifeDynamicObject* object, u32& object_count);

			void					SerializeElem(ISaveObject& Object, std::pair<ALife::_OBJECT_ID, CSE_ALifeDynamicObject*>& elem);
			u32 m_serializable_object_count = 0;

public:
	static	CSE_ALifeDynamicObject	*get_object				(IReader &file_stream);
	static	CSE_ALifeDynamicObject* get_object(ISaveObject& Object);

public:
									CALifeObjectRegistry	(LPCSTR section);
	virtual							~CALifeObjectRegistry	();
	virtual	void					save					(IWriter &memory_stream);
			void					load					(IReader &file_stream);
			//virtual	void			Save(CSaveObjectSave* Object) const;
			//void					Load(CSaveObjectLoad* Object);
			virtual void			Serialize(ISaveObject& Object);
	IC		void					add						(CSE_ALifeDynamicObject *object);
	IC		void					remove					(const ALife::_OBJECT_ID &id, bool no_assert = false);
	IC		CSE_ALifeDynamicObject	*object					(const ALife::_OBJECT_ID &id, bool no_assert = false) const;
	IC		const OBJECT_REGISTRY	&objects				() const;
	IC		OBJECT_REGISTRY			&objects				();
	const xr_vector<CSE_ALifeDynamicObject*> objects_vec() const;
};

#include "alife_object_registry_inline.h"