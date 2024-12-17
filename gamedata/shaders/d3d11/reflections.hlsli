#ifndef reflections_h_2134124_inc
#define reflections_h_2134124_inc

// Screen Space Sky Reflections off
#define SKYBLED_FADE 1

float get_depth_fast(float2 tc)
{
    float P = s_position.SampleLevel(smp_rtlinear, tc, 0).x;
    return depth_unpack.x / (P - depth_unpack.y);
}

float3 gbuf_unpack_position(float2 uv)
{
    float depth = get_depth_fast(uv);
    uv = uv * 2.0f - 1.0f;
    return float3(uv * pos_decompression_params.xy, 1.0f) * depth;
}

float2 gbuf_unpack_uv(float3 position)
{
    position.xy /= pos_decompression_params.xy * position.z;
    return saturate(position.xy * 0.5 + 0.5);
}

float4 ScreenSpaceLocalReflections(float3 Point, float3 Reflect)
{
    float2 ReflUV = 0.0;
    float3 HitPos, TestPos;
    float L = 0.025f, DeltaL = 0.0f;

    float Fade = saturate(dot(Reflect, normalize(Point)) * 4.0f);

    if (Fade < 0.001f)
    {
        return 0.0f;
    }

    [unroll(15)]
    for (int i = 0; i < 15; i++)
    {
        TestPos = Point + Reflect * L;
        ReflUV = gbuf_unpack_uv(TestPos);
        HitPos = gbuf_unpack_position(ReflUV);
        if (all(min(min(1.f - ReflUV.x, ReflUV.x), min(1.f - ReflUV.y, ReflUV.y))))
        {
            L = length(Point - HitPos);
        }
        else
        {
            return 0.0f;
        }
    }

    DeltaL = length(HitPos) - length(Point);
    Fade *= step(-0.4f, DeltaL);

    float Attention = GetBorderAtten(ReflUV, 0.125f);
    ReflUV -= s_velocity.SampleLevel(smp_rtlinear, ReflUV, 0).xy * float2(0.5f, -0.5f);
    Fade *= min(Attention, GetBorderAtten(ReflUV, 0.125f));

#ifdef SKYBLED_FADE
    float Fog = saturate(length(HitPos) * fog_params.w + fog_params.x);
    Fade *= 1.f - Fog * Fog;
#endif

    float3 Color = s_image.SampleLevel(smp_rtlinear, ReflUV, 0).xyz;
    return float4(Color, Fade);
}

float4 calc_reflections(float2 pos2d, float zpos, float3 vreflect)
{
    float3 Point = zpos * float3(pos2d * pos_decompression_params.zw - pos_decompression_params.xy, 1.0f);
    return ScreenSpaceLocalReflections(Point, mul((float3x3)m_V, vreflect));
}
#endif
