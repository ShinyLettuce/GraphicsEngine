#include "Common.hlsli"

PixelOutput main(PixelInputType input)
{
    uint w, h, ml;
    aRenderTexture.GetDimensions(0, w, h, ml);
    
    float3 r = float3(0.4f, 0.7f, 0.6f);
    
    PixelOutput output;
    output.color.rgb = aRenderTexture.SampleLevel(aSampler, input.screenPosition.xy / float2(w, h), 4).rgb * r; // + float3(0.1f, 0.3f, 0.4f);
    output.color.a = 1.0f;
    
    return output;
}
