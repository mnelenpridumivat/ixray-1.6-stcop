#pragma once
#include "Save/SaveObject.h"

class CArtefact;

class CArtContainer
{

protected:
	size_t					m_iContainerSize;
	xr_vector<CArtefact*>	m_sArtefactsInside;

public:

	CArtContainer(void);
	~CArtContainer(void);

	void			Load(LPCSTR section);

	void			save(NET_Packet& output_packet);
	void			load(IReader& input_packet);

	/*virtual void Save(CSaveObjectSave* Object) const;
	virtual void Load(CSaveObjectLoad* Object);*/
	virtual void Serialize(ISaveObject& Object);

	u32				Cost() const;
	float			Weight() const;

	size_t					GetContainerSize() const { return m_iContainerSize; }
	void					SetContainerSize(size_t new_size) { m_iContainerSize = new_size; }
	xr_vector<CArtefact*>	GetArtefactsInside() { return m_sArtefactsInside; }
	bool					IsFull() const { return m_sArtefactsInside.size() >= m_iContainerSize; }

	virtual	bool CanStoreArt(CArtefact* art);

	void					PutArtefactToContainer(const CArtefact& artefact);
	void					TakeArtefactFromContainer(CArtefact* artefact);
};

