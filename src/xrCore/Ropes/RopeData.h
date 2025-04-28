#pragma once

struct SRopeVertex
{
    Fvector Location; 
};

struct SRopeInputData
{
    
};

struct SRopeData
{
    float m_lenght = 1.0f;
    u64 m_segments = 5;
    u16 m_ring_segments = 6;

    xr_vector<xr_unique_ptr<SRopeVertex>> m_vertices;
    xr_vector<xr_unique_ptr<SRopeVertex>> m_vertices2;
};
