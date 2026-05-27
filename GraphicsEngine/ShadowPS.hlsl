#include "Common.hlsli"

#define STEP_SIZE 0.01

float2 ScaledUv(float2 p)
{
    float2 uv = (p / 128.0f + 0.5f);
    uv.y = 1.0f - uv.y;
    return uv;
}

float2 main(ShadowPixelInputType input) : SV_TARGET
{
    float2 color;

    float softShadow = 1.0f;
    float k = 2.0f;
    
    float h0 = aTexture.Sample(aSampler, input.uv).r;
  
    float3 ro = float3(input.worldPosition.x, h0 + 0.01f, input.worldPosition.y);
    
    float3 rd = normalize(-float3(0.0f, 0.0f, 1.0f));
    float t = 0.0f;
    float s = 0.0f;
    for (int i = 0; i < 128; ++i)
    {
        float3 p = ro + rd * t;
        
        float2 uv = ScaledUv(p.xz);
        float h1 = aTexture.Sample(aSampler, uv).r;
        
        s = p.y - h1;
        t += STEP_SIZE;
        
        softShadow = min(softShadow, k * s / t);
        
        if (s <= 0.0f)
        {
            break;
        }
    }
    
    //if (s <= 0.0f)
    //{
    //    color = float2(1.0f, 0.0f);
    //}
    //else
    //{
    //    color = float2(0.0f, 1.0f);
    //}
   
    //color = saturate(ro.xz) * (ro.y / MAX_PLANE_DISPLACEMENT);
    
    color.r = softShadow;
    
    return color;
    
    
}