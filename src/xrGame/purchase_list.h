////////////////////////////////////////////////////////////////////////////
//	Module 		: purchase_list.h
//	Created 	: 12.01.2006
//  Modified 	: 12.01.2006
//	Author		: Dmitriy Iassenev
//	Description : purchase list class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrCore/associative_vector.h"

class CInventoryOwner;
class CGameObject;

class CPurchaseList {
public:
	typedef associative_vector<shared_str,float>	DEFICITS;

private:
	DEFICITS				m_deficits;

	xr_hash_map<shared_str, u32> spawned_content = {};

private:
			void			process			(const CGameObject &owner, const shared_str &name, const u32 &count, const float &probability);

public:
			void			process			(CInifile &ini_file, LPCSTR section, CInventoryOwner &owner);
			void CopyList(xr_hash_map<shared_str, u32>& target);
			void AddItemToList(shared_str item);
			void RemoveItemFromList(shared_str item);

public:
	IC		void			deficit			(const shared_str &section, const float &deficit);
	IC		float			deficit			(const shared_str &section) const;
	IC		const DEFICITS	&deficits		() const;
};

#include "purchase_list_inline.h"