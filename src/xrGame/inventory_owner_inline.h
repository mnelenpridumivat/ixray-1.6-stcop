#include "InventoryOwner.h"
#pragma once

IC	CTradeParameters &CInventoryOwner::trade_parameters	() const
{
	VERIFY	(m_trade_parameters);
	return	(*m_trade_parameters);
}

IC	bool CInventoryOwner::can_barter() const {
	return m_barter_parameters;
}

IC CTradeParameters* CInventoryOwner::barter_parameters() const
{
	VERIFY(m_barter_parameters);
	return m_barter_parameters;
}

inline IC CPurchaseList& CInventoryOwner::trade_purchase_list() const
{
	VERIFY(m_purchase_list);
	return *m_purchase_list;
}

inline IC CPurchaseList& CInventoryOwner::barter_purchase_list() const
{
	VERIFY(m_barter_purchase_list);
	return *m_barter_purchase_list;
}
