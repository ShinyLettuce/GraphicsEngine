#include "Common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    float diffuse = dot(input.normal, normalize(float3(1.0f, -1.0f, 0.5f))) * 0.5f + 0.5f;
    float3 materialColor = float3(1.0f, 1.0f, 1.0f);
    float3 groundColor = float3(0.f, 1.f, 0.f);
    float3 skyColor = float3(0.f, 0.f, 1.f);
    float3 ambientColor = materialColor * ((0.5f + 0.5f * input.normal.y) * skyColor + (0.5f - 0.5f * input.normal.y) * groundColor);
    float ambientFactor = 0.3f;
    float diffuseFactor = 0.7f;
    result.color.rgb = ambientFactor * ambientColor + diffuse * diffuseFactor;
    
    return result;
}