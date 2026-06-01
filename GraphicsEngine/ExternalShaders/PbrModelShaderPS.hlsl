#include "../Common.hlsli"
#include "PBRFunctions.hlsli"

// Origin: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

float3 SamplePackedNormal(Texture2D texture2d, SamplerState samplerState, float2 uv)
{
    float3 n = texture2d.Sample(samplerState, uv).rgg;
    n.xy = 2.0f * n.xy - 1.0f;
    n.z = sqrt(1.0f - saturate(n.x * n.x + n.y * n.y));
    return normalize(n);
}

PixelOutput main(PixelInputType input)
{
    PixelOutput result;

    float2 scaledUV = input.uv;
	
    float3 toEye = normalize(eyePosition - input.worldPosition.xyz);
    
    float slopeBlend = smoothstep(0.7f, 1.0f, input.normal.y);
    float heightBlend = smoothstep(0.0f, 1.5f, input.worldPosition.y);
    
    float4 grassAlbedo = aGrassTexture.Sample(aSampler, scaledUV);
    float4 rockAlbedo = aRockTexture.Sample(aSampler, scaledUV);
    float4 snowAlbedo = aSnowTexture.Sample(aSampler, scaledUV);
    
    float4 albedo = lerp(rockAlbedo, lerp(grassAlbedo, snowAlbedo, heightBlend), slopeBlend);

    float3 grassNormal = SamplePackedNormal(aGrassNormalTexture, aSampler, scaledUV);
    float3 rockNormal = SamplePackedNormal(aRockNormalTexture, aSampler, scaledUV);
    float3 snowNormal = SamplePackedNormal(aSnowNormalTexture, aSampler, scaledUV);
    
    float3 normal = lerp(rockNormal, lerp(grassNormal, snowNormal, heightBlend), slopeBlend);

    float3x3 TBN = float3x3(
		normalize(input.tangent),
		normalize(-input.bitangent),
		normalize(input.normal));

	// Can save an instruction here by instead doing
	// normalize(mul(normal, TBN)); It works because
	// TBN is a 3x3 and therefore TBN^T is the same
	// as TBN^-1. However, it is considered good form
	// to do this.
    TBN = transpose(TBN);
    float3 pixelNormal = normalize(mul(TBN, normal));

	// TGA Channel Pack. ORM.
	// Metalness, Roughness, Emissive, Emissive Strength (opt).

    float3 grassMaterial = aGrassMaterialTexture.Sample(aSampler, scaledUV);
    float3 rockMaterial = aRockMaterialTexture.Sample(aSampler, scaledUV);
    float3 snowMaterial = aSnowMaterialTexture.Sample(aSampler, scaledUV);
    
    float3 material = lerp(rockMaterial, lerp(grassMaterial, snowMaterial, heightBlend), slopeBlend);

    float ambientOcclusion = material.r;
    float metalness = material.b;
    float roughness = material.g;

    float3 AmbientLightColor = float3(0.2f, 0.3f, 0.4f);

    float3 specularColor = lerp(0.07f, albedo.rgb, metalness);
    float3 diffuseColor = lerp(0.0f, albedo.rgb, 1.0f - metalness);

    float3 ambiance = AmbientLightColor * EvaluateAmbiance(
		aCubeMap, aSampler, pixelNormal, input.normal,
		toEye, roughness,
		ambientOcclusion, diffuseColor, specularColor
	);
    
    float3 directionalLight;

    float DirectionalLightSoftness = 0.0f;
    float3 DirectionalLightColor = float3(0.6f, 0.45f, 0.3f);
    float3 DirectionalLightTransform = normalize(float3(1.0f, -0.3f, 0.0f));
    
    if (DirectionalLightSoftness == 0.0f)
    {
        directionalLight = EvaluateDirectionalLight(
			diffuseColor, specularColor, pixelNormal, roughness,
			DirectionalLightColor, DirectionalLightTransform, -toEye);
    }
    else
    {
        directionalLight = EvaluateSoftDirectionalLight(
			diffuseColor, specularColor, pixelNormal, roughness, DirectionalLightSoftness,
			DirectionalLightColor, DirectionalLightTransform, -toEye);
    }
	
    float3 radiance = (directionalLight * (1.0f - aShadowTexture.Sample(aSampler, scaledUV).g) + ambiance * (1.0f - aShadowTexture.Sample(aSampler, scaledUV).r));

    result.color.rgb = tonemap_s_gamut3_cine(radiance);
    result.color.a = albedo.a;
    
    //result.color = 0.0f;
    //result.color.g = aShadowTexture.Sample(aSampler, scaledUV).g;
    //result.color.r = aShadowTexture.Sample(aSampler, scaledUV).r;
    
    return result;
}

