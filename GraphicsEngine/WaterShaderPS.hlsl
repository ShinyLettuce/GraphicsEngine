#include "Common.hlsli"

PixelOutput main(PixelInputType input)
{
    uint w, h, ml;
    aRenderTexture.GetDimensions(0, w, h, ml);
    
    float3 r = float3(0.4f, 0.7f, 0.6f);
    
    float3 color = aRenderTexture.SampleLevel(aSampler, input.screenPosition.xy / float2(w, h), 4).rgb;
    
    PixelOutput output;
    output.color.rgb = color * r;
    output.color.a = 1.0f;
    
    output.color = 0.0f;
    output.color.rg = input.uv;
    
    return output;
}
