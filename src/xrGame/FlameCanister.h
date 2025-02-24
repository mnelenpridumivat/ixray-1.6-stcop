#pragma once

#include "CartrigeParam.h"
#include "inventory_item_object.h"
#include "RepackerInterface.h"

class CFlameCanister :
	public CInventoryItemObject,
	public IRepackerInterface {
	typedef CInventoryItemObject		inherited;
public:
	CFlameCanister(void);
	~CFlameCanister(void) override;

	CFlameCanister* cast_flame_canister() override { return this; }

	void					Load(LPCSTR section) override;
	BOOL					net_Spawn(CSE_Abstract* DC) override;
	void					net_Destroy() override;
	void					net_Export(NET_Packet& P) override;
	void					net_Import(NET_Packet& P) override;
	void					OnH_B_Chield() override;
	void					OnH_B_Independent(bool just_before_destroy) override;
	void					UpdateCL() override;
	void					renderable_Render() override;

	bool					Useful() const override;
	float					Weight() const override;
	u32						Cost() const override;

	virtual bool Repack(PIItem Other) override;
	virtual bool IsValid() const override;

	SCartridgeParam cartridge_param;

public:
	CInventoryItem* can_make_killing(const CInventory* inventory) const override;
};
