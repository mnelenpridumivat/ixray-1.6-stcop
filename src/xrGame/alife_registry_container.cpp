////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_container.cpp
//	Created 	: 01.07.2004
//  Modified 	: 01.07.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry container class
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "alife_registry_container.h"
#include "object_interfaces.h"
#include "alife_space.h"
#include "../xrCore/object_type_traits.h"

template <typename T1, typename _T2, typename Head>
struct CRegistryHelperLoad {
	typedef typename std::remove_reference<_T2>::type T2;

	template <bool loadable>
	IC	static void do_load(T1 *self, T2 &p1)
	{
	}

	template <>
	IC	static void do_load<true>(T1 *self, T2 &p1)
	{
		self->Head::load(p1);
	}

	IC	static void process(T1 *self, T2 &p1)
	{
		do_load<std::is_base_of<IPureLoadableObject<T2>,Head>::value>(self,p1);
	}
};

template <typename T1, typename _T2, typename Head>
struct CRegistryHelperSave {
	typedef typename std::remove_reference<_T2>::type T2;

	template <bool loadable>
	IC	static void do_save(T1 *self, T2 &p1)
	{
	}

	template <>
	IC	static void do_save<true>(T1 *self, T2 &p1)
	{
		self->Head::save(p1);
	}

	IC	static void process(T1 *self, T2 &p1)
	{
		do_save<std::is_base_of<IPureSavableObject<T2>,Head>::value>(self,p1);
	}
};

template <template <typename _1, typename _2, typename _3> class helper, typename T1, typename T2, typename TList>
class CRegistryHelperProcess
{
private:
	ASSERT_TYPELIST(TList);

	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

public:
	template <typename _1>
	IC	static void go_process(T1 *self, T2 p1)
	{
		CRegistryHelperProcess<helper,T1,T2,Tail>::process(self,p1);
	}

	template <>
	IC	static void go_process<Loki::NullType>(T1 *self, T2 p1)
	{
	}

	IC	static void process(T1 *self, T2 p1)
	{
		go_process<Tail>(self,p1);
		helper<T1,T2,Head>::process(self,p1);
	}
};

void CALifeRegistryContainer::load(IReader &file_stream)
{
	R_ASSERT2					(file_stream.find_chunk(REGISTRY_CHUNK_DATA),"Can't find chunk REGISTRY_CHUNK_DATA!");
	CRegistryHelperProcess<
		CRegistryHelperLoad,
		CALifeRegistryContainer,
		IReader&,
		TYPE_LIST
	>::process					(this,file_stream);
}

void CALifeRegistryContainer::save(IWriter &memory_stream)
{
	memory_stream.open_chunk	(REGISTRY_CHUNK_DATA);
	CRegistryHelperProcess<
		CRegistryHelperSave,
		CALifeRegistryContainer,
		IWriter&,
		TYPE_LIST
	>::process					(this,memory_stream);
	memory_stream.close_chunk	();
}

void CALifeRegistryContainer::load(CSaveObjectLoad* Object)
{
	Object->BeginChunk("CALifeRegistryContainer");
	{
		CInfoPortionRegistry::load(Object);
		CRelationRegistry::load(Object);
		CEncyclopediaRegistry::load(Object);
		CGameNewsRegistry::load(Object);
		CSpecificCharacterRegistry::load(Object);
		CMapLocationRegistry::load(Object);
		CGameTaskRegistry::load(Object);
		CActorStatisticRegistry::load(Object);
	}
	Object->EndChunk();
}

void CALifeRegistryContainer::save(CSaveObjectSave* Object) const
{
	Object->BeginChunk("CALifeRegistryContainer");
	{

	}
	Object->EndChunk();
}
