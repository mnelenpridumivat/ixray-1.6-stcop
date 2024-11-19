#include "stdafx.h"
#include "ArtContainer.h"
#include "Artefact.h"
#include "level.h"
#include "Actor.h"

float af_from_container_charge_level = 1.0f;
int af_from_container_rank = 1;
CArtContainer* m_LastAfContainer = nullptr;

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
        CArtefact* artefact = xr_new<CArtefact>();
        shared_str section;

        load_data(section, packet);

        artefact->Load(section.c_str());

        artefact->load(packet);

        m_sArtefactsInside.push_back(artefact);
    }
}

bool CArtContainer::CanStoreArt(CArtefact* art)
{
    return true;
}

void CArtContainer::PutArtefactToContainer(const CArtefact& artefact)
{
    CArtefact* af = xr_new<CArtefact>(artefact);

    af->m_bInContainer = true;

    m_sArtefactsInside.push_back(af);
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

            af_from_container_charge_level = artefact->GetCurrentChargeLevel();
            af_from_container_rank = artefact->GetCurrentAfRank();
            m_LastAfContainer = this;

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
