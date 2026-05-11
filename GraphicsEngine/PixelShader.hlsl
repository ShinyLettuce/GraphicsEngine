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
    TBN = transpose(TBN);
    
    float3 grassNormal = aGrassNormalTexture.Sample(aSampler, input.uv).rgb;
    grassNormal.z = sqrt(1.0f - (grassNormal.x * grassNormal.x - grassNormal.y * grassNormal.y));
    grassNormal = grassNormal * 2.0 - 1.0;

    
    float3 rockNormal = aRockNormalTexture.Sample(aSampler, input.uv).rgb;
    rockNormal.z = sqrt(1.0f - (rockNormal.x * rockNormal.x - rockNormal.y * rockNormal.y));
    rockNormal = rockNormal * 2.0 - 1.0;

    
    float3 snowNormal = aSnowNormalTexture.Sample(aSampler, input.uv).rgb;
    snowNormal.z = sqrt(1.0f - (snowNormal.x * snowNormal.x - snowNormal.y * snowNormal.y));
    snowNormal = snowNormal * 2.0 - 1.0;

    
    float slopeBlend = smoothstep(0.7f, 1.0f, input.normal.y);
    float heightBlend = smoothstep(-0.05f, 0.25f, input.worldPosition.y);
    
    float3 grassColor = aGrassTexture.Sample(aSampler, input.uv);
    float3 rockColor = aRockTexture.Sample(aSampler, input.uv);
    float3 snowColor = aSnowTexture.Sample(aSampler, input.uv);
    
    float3 color = lerp(rockColor, lerp(grassColor, snowColor, heightBlend), slopeBlend).rgb;
    float3 normal = lerp(rockNormal, lerp(grassNormal, snowNormal, heightBlend), slopeBlend);
    normal = normalize(mul(TBN, normal));
    
    float diffuse = saturate(dot(normal, normalize(float3(cos(time), 0.0f, sin(time)))));
    
    float3 ambient = float3(0.1f, 0.1f, 0.1f);
    
    result.color.rgb = lerp(float3(0.1f, 0.2f, 0.3f), float3(1.f, 1.f, 0.9f), diffuse + ambient) * color;
    
    return result;
}