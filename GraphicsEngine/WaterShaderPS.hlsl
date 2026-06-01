#include "Common.hlsli"
#include "ExternalShaders/PBRFunctions.hlsli"

PixelOutput main(PixelInputType input)
{
    uint w, h, ml;
    aRenderTexture.GetDimensions(0, w, h, ml);
    
    float3 toEye = normalize(eyePosition - input.worldPosition.xyz);
    
    float fresnel = Fresnel_Schlick(
        float3(0.25f, 0.25f, 0.25f),
        float3(0.0f, 1.0f, 0.0f),
        toEye);
    
    float3 reflection = aRenderTexture.SampleLevel(aSampler, input.screenPosition.xy / float2(w, h), 4.0f).rgb;
    
    PixelOutput output;
    output.color.rgb = reflection * fresnel;
    output.color.a = 1.0f;
    
    return output;
}
