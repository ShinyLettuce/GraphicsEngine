#include "Common.hlsli"

#define MAX_PLANE_DISPLACEMENT 8.0
#define STEP_SIZE 0.01

float2 main(ShadowPixelInputType input) : SV_TARGET
{
    float2 color;

    float h0 = aTexture.Sample(aSampler, input.uv) * MAX_PLANE_DISPLACEMENT;
    
    float3 ro = float3(lerp(16.0f, 144.0f, input.uv.x), h0 + 0.01f, lerp(16.0f, 144.0f, input.uv.y));
    float3 rd = normalize(float3(0.0f, 0.0f, -1.0f));
    float t = 0.0f;
    float s = 0.0f;
    for (int i = 0; i < 128; ++i)
    {
        float3 p = ro + rd * t;
        
        float2 uv = (p.xz - 16.0f) / 128.0f;
        float h1 = aTexture.Sample(aSampler, float2(1.0f - uv.x, uv.y)) * MAX_PLANE_DISPLACEMENT;
        
        s = p.y - h1;
        t += STEP_SIZE;
        
        if (s <= 0.0f)
        {
            break;
        }
    }
    
    if (s <= 0.0f)
    {
        color = float2(1.0f, 0.0f);
    }
    else
    {
        color = float2(0.0f, 1.0f);
    }
   
    
    color = frac(ro.xz);
    
    return color;
    
    
}