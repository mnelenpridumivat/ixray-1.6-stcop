#pragma once

#include "inventory_item_object.h"
#include "ArtContainer.h"

class CArtefactContainer : public CInventoryItemObject, public CArtContainer
{
	typedef CInventoryItemObject inherited;

public:

	CArtefactContainer(void);
	~CArtefactContainer(void) override;

	void			Load						(LPCSTR section) override;
	BOOL			net_Spawn					(CSE_Abstract* DC) override;

	void			save						(NET_Packet& output_packet) override;
	void			load						(IReader& input_packet) override;

	//virtual void Save(CSaveObjectSave* Object) const override;
	//virtual void Load(CSaveObjectLoad* Object) override;
	virtual void Serialize(ISaveObject& Object) override;

	u32				Cost						() const override;
	float			Weight						() const override;
};
