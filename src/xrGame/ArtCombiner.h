#pragma once
#include "ArtContainer.h"
#include "Artefact.h"
class CArtCombiner :
    public CArtefact,
	public CArtContainer
{
	typedef CArtefact inherited;

public:

	void			Load(LPCSTR section) override;
	BOOL			net_Spawn(CSE_Abstract* DC) override;

	void			save(NET_Packet& output_packet) override;
	void			load(IReader& input_packet) override;

	u32				Cost() const override;
	float			Weight() const override;

	virtual	bool CanStoreArt(CArtefact* art) override;

	virtual bool CheckInventoryIconItemSimilarity(CInventoryItem* other) override;

	virtual float GetHealthPower() const override;
	virtual float GetRadiationPower() const override;
	virtual float GetSatietyPower() const override;
	virtual float GetPowerPower() const override;
	virtual float GetBleedingPower() const override;
	virtual float AdditionalInventoryWeight() const override;
	virtual float GetJumpPower() const override;
	virtual float GetWalkPower() const override;

	virtual float GetImmunity(ALife::EHitType hit_type) override;

	virtual float ArtAffectHit(float power, ALife::EHitType hit_type) override;

};

