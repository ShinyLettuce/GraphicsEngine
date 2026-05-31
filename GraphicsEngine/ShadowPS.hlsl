#include "Common.hlsli"

#define AO_SAMPLE_COUNT 1024

static const float PI = 3.14159265f;

float2 FibonacciLattice(int i, int N)
{
    float PHI = 0.5 * (sqrt(5.) + 1.);
    return float2((float(i) + 0.5f) / float(N), fmod(float(i) / PHI, 1.));
}

float3 FibonacciSphere(int i, int N)
{
    float2 xy = FibonacciLattice(i, N);
    float2 pt = float2(2.f * PI * xy.y, acos(2.f * xy.x - 1.f) - PI * 0.5f);
    return float3(cos(pt.x) * cos(pt.y), sin(pt.x) * cos(pt.y), sin(pt.y));
}

float2 ScaledUv(float2 p)
{
    float2 uv = (p / 128.0f + 0.5f);
    return saturate(float2(uv.x, 1.0f - uv.y));
}

bool RayMarch(float3 ro, float3 rd, float mint, float maxt, float stepSize)
{
    float t = mint;
    float s = 0.0f;
    
    [fastopt]
    [loop]
    for (int i = 0; i < 2048; ++i)
    {
        float3 p = ro + rd * t;
        
        float2 uv = ScaledUv(p.xz);
        if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f)
        {
            break;
        }
        
        float h1 = aTexture.Sample(aSampler, uv).r * 16.0f;
        
        s = p.y - h1;
        t += stepSize;
        
        if (s <= 0.0f || t > maxt)
        {
            break;
        }
    }
    
    return s <= 0.0f;
}

float2 main(ShadowPixelInputType input) : SV_TARGET
{
    float2 color = 0.0f;
    
    float shadow = 0.0f;
    float ao = 0.0f;
    
    float3 ro = float3(input.worldPosition.x, 0.0f, input.worldPosition.y);
    
    {
        float2 uv = ScaledUv(ro.xz);
        if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f)
        {
            return 1.0f;
        }
    
        float h0 = aTexture.Sample(aSampler, uv).r * 16.0f;
        
        ro.y = h0 + 0.1f;
    }
    
    // Shadow pass
    {
        float3 rd = normalize(float3(1.0f, 0.3f, 0.0f));  
        shadow = RayMarch(ro, rd, 0.01f, 128.0f, 0.1f);
    }

    // Ao pass
    {
        for (int i = 0; i < AO_SAMPLE_COUNT; ++i)
        {
            float3 rd = normalize(FibonacciSphere(i, AO_SAMPLE_COUNT));
            ao += RayMarch(ro, rd, 0.1f, 0.7f, 0.01f);
        }
        
        ao /= AO_SAMPLE_COUNT;
        ao = saturate(ao * ao * ao * ao);
    }

    color.r = ao;
    color.g = shadow;
    
    return color;
}