////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_abstract_registry_inline.h
//	Created 	: 30.06.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife abstract registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace SaveSystemDefined {

	//template<typename K, typename V>
	//void Save(CSaveObjectSave* Obj, K Key, const V& Value);

	//template<typename K, typename V>
	//void Load(CSaveObjectLoad* Obj, K& Key, V& Value);

	template<typename K, typename V>
	void Serialize(ISaveObject& Object, xr_map<K, V>& Value);
}

#define TEMPLATE_SPECIALIZATION template <typename _index_type, typename _data_type>
#define CSALifeAbstractRegistry CALifeAbstractRegistry<_index_type,_data_type>

TEMPLATE_SPECIALIZATION
IC	CSALifeAbstractRegistry::CALifeAbstractRegistry	()
{
}

TEMPLATE_SPECIALIZATION
CSALifeAbstractRegistry::~CALifeAbstractRegistry		()
{
	delete_data		(m_objects);
}

TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry::save					(IWriter &memory_stream)
{
	save_data		(m_objects,memory_stream);
}

TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry ::load					(IReader &file_stream)
{
	load_data		(m_objects,file_stream);
}

/*TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry::save(CSaveObjectSave* Object) const
{
	Object->GetCurrentChunk()->WriteArray(m_objects.size());
	{
		for (const auto& elem : m_objects) {
			Object->BeginChunk("CSALifeAbstractRegistry::elem");
			{
				SaveSystemDefined::Save<_index_type, _data_type>(Object, elem.first, elem.second);
			}
			Object->EndChunk();
		}
	}
	Object->GetCurrentChunk()->EndArray();
}

TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry::load(CSaveObjectLoad* Object)
{
	u64 ArraySize;
	Object->GetCurrentChunk()->ReadArray(ArraySize);
	{
		for (u64 i = 0; i < ArraySize; ++i) {		
			std::pair< _index_type, _data_type>&& temp = std::pair< _index_type, _data_type>();
			Object->BeginChunk("CSALifeAbstractRegistry::elem");
			{
				SaveSystemDefined::Load<_index_type, _data_type>(Object, temp.first, temp.second);
			}
			Object->EndChunk();
			m_objects.emplace(temp);
		}
	}
	Object->GetCurrentChunk()->EndArray();
}*/

TEMPLATE_SPECIALIZATION
void CSALifeAbstractRegistry::serialize(ISaveObject& Object)
{
	SaveSystemDefined::Serialize(Object, m_objects);
	/*Object << m_objects;
	Object->GetCurrentChunk()->WriteArray(m_objects.size());
	{
		for (const auto& elem : m_objects) {
			Object->BeginChunk("CSALifeAbstractRegistry::elem");
			{
				SaveSystemDefined::Save<_index_type, _data_type>(Object, elem.first, elem.second);
			}
			Object->EndChunk();
		}
	}
	Object->GetCurrentChunk()->EndArray();*/
}

TEMPLATE_SPECIALIZATION
IC	const typename CSALifeAbstractRegistry::OBJECT_REGISTRY &CSALifeAbstractRegistry::objects	() const
{
	return			(m_objects);
}

TEMPLATE_SPECIALIZATION
IC	void CSALifeAbstractRegistry::add				(const _index_type &index, _data_type &data, bool no_assert)
{
	const_iterator	I = objects().find(index);
	if (I != objects().end()) {
		THROW2		(no_assert,"Specified object has been already found in the specified registry!");
		return;
	}
	m_objects.insert(std::make_pair(index,data));
}

TEMPLATE_SPECIALIZATION
IC	void CSALifeAbstractRegistry::remove			(const _index_type &index, bool no_assert)
{
	iterator		I = m_objects.find(index);
	if (I == objects().end()) {
		THROW2		(no_assert,"Specified object hasn't been found in the specified registry!");
		return;
	}
	m_objects.erase	(I);
}

TEMPLATE_SPECIALIZATION
IC	_data_type *CSALifeAbstractRegistry::object	(const _index_type &index, bool no_assert)
{
	iterator		I = m_objects.find(index);
	if (I == objects().end()) {
		THROW2		(no_assert,"Specified object hasn't been found in the specified registry!");
		return		(0);
	}
	return			(&(*I).second);
}

#undef TEMPLATE_SPECIALIZATION