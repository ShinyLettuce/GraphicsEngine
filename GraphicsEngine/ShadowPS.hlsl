#include "Common.hlsli"

#define STEP_SIZE 0.01

float2 ScaledUv(float2 p)
{
    float2 uv = (p / 128.0f + 0.5f);
    return uv;
}

float2 main(ShadowPixelInputType input) : SV_TARGET
{
    float2 color = 0.0f;
    
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
    
    float3 rd = normalize(-float3(-1.0f, -0.3f, 0.0f));
    float t = 0.0f;
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
        t += STEP_SIZE;
        
        if (s <= 0.0f)
        {
            break;
        }
    }
    
    color.g = 0.0f;
    color.r = s < 0.0f;
    
    return color;
}