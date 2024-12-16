#include "stdafx.h"
#include "ArtContainer.h"
#include "Artefact.h"
#include "level.h"
#include "Actor.h"

//float af_from_container_charge_level = 1.0f;
//int af_from_container_rank = 1;
//CArtContainer* m_LastAfContainer = nullptr;

CArtContainer::CArtContainer()
{
    m_iContainerSize = 1;
    m_sArtefactsInside.clear();
}

CArtContainer::~CArtContainer()
{
}

void CArtContainer::Load(LPCSTR section)
{
    m_iContainerSize = pSettings->r_s32(section, "container_size");
}

void CArtContainer::save(NET_Packet& packet)
{
    u32 numArtefacts = m_sArtefactsInside.size();
    save_data(numArtefacts, packet);

    for (const auto& artefact : m_sArtefactsInside)
    {
        shared_str section = artefact->cNameSect();
        save_data(section, packet);

        artefact->save(packet);
    }
}

void CArtContainer::load(IReader& packet)
{
    u32 numArtefacts;
    load_data(numArtefacts, packet);

    m_sArtefactsInside.clear();

    for (u32 i = 0; i < numArtefacts; ++i)
    {
        CArtefact* artefact = new CArtefact();
        shared_str section;

        load_data(section, packet);

        artefact->Load(section.c_str());

        artefact->load(packet);

        m_sArtefactsInside.push_back(artefact);
    }
}

/*void CArtContainer::Save(CSaveObjectSave* Object) const
{
    Object->BeginChunk("CArtContainer");
    {
        Object->GetCurrentChunk()->WriteArray(m_sArtefactsInside.size());
        {
            for (const auto& artefact : m_sArtefactsInside)
            {
                shared_str section = artefact->cNameSect();
                Object->GetCurrentChunk()->w_stringZ(section);
                artefact->Save(Object);
            }
        }
        Object->GetCurrentChunk()->EndArray();
    }
    Object->EndChunk();
}

void CArtContainer::Load(CSaveObjectLoad* Object)
{
    Object->BeginChunk("CArtContainer");
    {
        u64 ArraySize;
        Object->GetCurrentChunk()->ReadArray(ArraySize);
        {
            for (u64 i = 0; i < ArraySize; ++i)
            {
                shared_str section;
                Object->GetCurrentChunk()->r_stringZ(section);
                CArtefact* artefact = new CArtefact();
                artefact->Load(section.c_str());
                artefact->Load(Object);
                m_sArtefactsInside.push_back(artefact);
            }
        }
        Object->GetCurrentChunk()->EndArray();
    }
    Object->EndChunk();
}*/

void CArtContainer::Serialize(ISaveObject& Object)
{
    Object.BeginChunk("CArtContainer");
    {
        // TODO: Reimplement this, storing arts like object in regulat inventory box
        /*u64 ArraySize;
        Object->GetCurrentChunk()->ReadArray(ArraySize);
        {
            for (u64 i = 0; i < ArraySize; ++i)
            {
                shared_str section;
                Object->GetCurrentChunk()->r_stringZ(section);
                CArtefact* artefact = new CArtefact();
                artefact->Load(section.c_str());
                artefact->Load(Object);
                m_sArtefactsInside.push_back(artefact);
            }
        }
        Object->GetCurrentChunk()->EndArray();*/
    }
    Object.EndChunk();
}

bool CArtContainer::CanStoreArt(CArtefact* art)
{
    return true;
}

void CArtContainer::PutArtefactToContainer(const CArtefact& artefact)
{
    //CArtefact* af = new CArtefact(artefact);

    //af->m_bInContainer = true;

    //m_sArtefactsInside.push_back(af);
}

void CArtContainer::TakeArtefactFromContainer(CArtefact* artefact)
{
    for (auto it = m_sArtefactsInside.begin(); it != m_sArtefactsInside.end();)
    {
        if (*it == artefact)
        {
            CArtefact* item_to_spawn = smart_cast<CArtefact*>(*it);
            it = m_sArtefactsInside.erase(it);

            Level().spawn_item(item_to_spawn->cNameSect().c_str(), Actor()->Position(), false, Actor()->ID());

            //af_from_container_charge_level = artefact->GetCurrentChargeLevel();
            //af_from_container_rank = artefact->GetCurrentAfRank();
            //m_LastAfContainer = this;

            return;
        }
        ++it;
    }
}

u32 CArtContainer::Cost() const
{
    u32 res = 0;

    for (const auto& artefact : m_sArtefactsInside)
        res += artefact->Cost();

    return res;
}

float CArtContainer::Weight() const
{
    float res = 0;

    for (const auto& artefact : m_sArtefactsInside)
        res += artefact->Weight();

    return res;
}
