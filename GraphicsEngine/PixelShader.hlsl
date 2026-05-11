#include "Common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    //float diffuse = dot(input.normal, normalize(float3(1.0f, -1.0f, 0.5f)));
    //float3 materialColor = float3(0.0f, 1.0f, 0.0f);
    //float3 groundColor = float3(0.f, 1.f, 0.f);
    //float3 skyColor = float3(0.f, 0.f, 1.f);
    //float3 ambientColor = materialColor * ((0.5f + 0.5f * input.normal.y) * skyColor + (0.5f - 0.5f * input.normal.y) * groundColor);
    //float ambientFactor = 0.2f;
    //float diffuseFactor = 1.0f;
    //result.color.rgb = ambientFactor * ambientColor + diffuse * diffuseFactor;
    //result.color.rgb *= saturate(aTexture.Sample(aSampler, input.uv).r)* 0.5f + 0.5f;
   
    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    
    float3 normal = aGrassNormalTexture.Sample(aSampler, input.uv).rgb;
    normal.z = sqrt(1.0f - normal.x * normal.x - normal.y * normal.y);
    normal = normal * 2.0 - 1.0;
    normal = normalize(mul(TBN, normal));
    
    float3 color = aGrassTexture.Sample(aSampler, input.uv);
    
    float diffuse = saturate(dot(normal, normalize(float3(cos(time), 0.0f, sin(time)))));
    
    result.color.rgb = color * diffuse;
    
    return result;
}