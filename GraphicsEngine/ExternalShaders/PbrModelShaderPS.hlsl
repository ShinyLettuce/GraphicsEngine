#include "../Common.hlsli"
#include "PBRFunctions.hlsli"

float3 SamplePackedNormal(Texture2D texture2d, SamplerState samplerState, float2 uv)
{
    float3 n = texture2d.Sample(samplerState, uv).xyy;
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
    float heightBlend = smoothstep(-0.05f, 0.25f, input.worldPosition.y);
    
    float4 grassAlbedo = aGrassTexture.Sample(aSampler, scaledUV);
    float4 rockAlbedo = aRockTexture.Sample(aSampler, scaledUV);
    float4 snowAlbedo = aSnowTexture.Sample(aSampler, scaledUV);
    
    float4 albedo = lerp(rockAlbedo, lerp(grassAlbedo, snowAlbedo, heightBlend), slopeBlend);

    float3 grassNormal = SamplePackedNormal(aGrassTexture, aSampler, scaledUV);   
    float3 rockNormal = SamplePackedNormal(aRockTexture, aSampler, scaledUV);
    float3 snowNormal = SamplePackedNormal(aSnowTexture, aSampler, scaledUV);
    
    float3 normal = lerp(rockNormal, lerp(grassNormal, snowNormal, heightBlend), slopeBlend).rgb;

    float3x3 TBN = float3x3(
		normalize(input.tangent.xyz),
		normalize(-input.bitangent.xyz),
		normalize(input.normal.xyz)
		);

	// Can save an instruction here by instead doing
	// normalize(mul(normal, TBN)); It works because
	// TBN is a 3x3 and therefore TBN^T is the same
	// as TBN^-1. However, it is considered good form
	// to do this.
    TBN = transpose(TBN);
    float3 pixelNormal = normalize(mul(TBN, normal));

	// TGA Channel Pack. ORM.
	// Metalness, Roughness, Emissive, Emissive Strength (opt).

    float3 grassMaterial = aGrassMaterialTexture.Sample(aSampler, scaledUV).rgb;
    float3 rockMaterial = aRockMaterialTexture.Sample(aSampler, scaledUV).rgb;
    float3 snowMaterial = aSnowMaterialTexture.Sample(aSampler, scaledUV).rgb;
    
    float3 material = lerp(rockMaterial, lerp(grassMaterial, snowMaterial, heightBlend), slopeBlend).rgb;

    float ambientOcclusion = material.r;
    float metalness = material.b;
    float roughness = material.g;
    
    //ambientOcclusion = 0.0f;
    //metalness = 1.0f;
    //roughness = 0.0f;

    float3 AmbientLightColor = 1.0f;
    
    float3 specularColor = lerp(0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp(0.00f, albedo.rgb, 1.0f - metalness);

    float3 ambiance = AmbientLightColor.rgb * EvaluateAmbiance(
		aCubeMap, aSampler, input.normal, normal,
		toEye, roughness,
		ambientOcclusion, diffuseColor, specularColor
	);
    
    float3 directionalLight;

    float DirectionalLightSoftness = 0.0f;
    float3 DirectionalLightColor = 1.0f;
    float3 DirectionalLightTransform = normalize(float3(cos(time), 1.0f, sin(time)));
    
    if (DirectionalLightSoftness == 0.0f)
    {
        directionalLight = EvaluateDirectionalLight(
			diffuseColor, specularColor, pixelNormal, roughness,
			DirectionalLightColor, DirectionalLightTransform, toEye);
    }
    else
    {
        directionalLight = EvaluateSoftDirectionalLight(
			diffuseColor, specularColor, pixelNormal, roughness, DirectionalLightSoftness,
			DirectionalLightColor, DirectionalLightTransform, toEye);
    }
	
    float3 radiance = directionalLight + ambiance;

    result.color.rgb = radiance * albedo.rgb;
    result.color.a = albedo.a;
    return result;
}

