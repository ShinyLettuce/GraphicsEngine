#include "../Common.hlsli"
#include "PBRFunctions.hlsli"

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
    
    //if (albedo.a <= alphaTestThreshold)
    //{
    //    discard;
    //    result.color = float4(0.f, 0.f, 0.f, 0.f);
    //    return result;
    //}

    float3 grassNormal = aGrassNormalTexture.Sample(aSampler, scaledUV).xyy;

    grassNormal.xy = 2.0f * grassNormal.xy - 1.0f;
    grassNormal.z = sqrt(1 - saturate(grassNormal.x * grassNormal.x + grassNormal.y * grassNormal.y));
    grassNormal = normalize(grassNormal);
    
    float3 rockNormal = aRockNormalTexture.Sample(aSampler, scaledUV).xyy;
    
    rockNormal.xy = 2.0f * rockNormal.xy - 1.0f;
    rockNormal.z = sqrt(1 - saturate(rockNormal.x * rockNormal.x + rockNormal.y * rockNormal.y));
    rockNormal = normalize(rockNormal);
    
    float3 snowNormal = aSnowNormalTexture.Sample(aSampler, scaledUV).xyy;
    
    snowNormal.xy = 2.0f * snowNormal.xy - 1.0f;
    snowNormal.z = sqrt(1 - saturate(snowNormal.x * snowNormal.x + snowNormal.y * snowNormal.y));
    snowNormal = normalize(snowNormal);
    
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

    //float3 fx = fxTexture.Sample(defaultSampler, scaledUV).rgb;

    //float emissive = fx.r;
	
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

    //float3 ambiance = AmbientLightColor.rgb * EvaluateAmbiance(
	//	environmentTexture, pixelNormal, input.normal.xyz,
	//	toEye, roughness,
	//	ambientOcclusion, diffuseColor, specularColor
	//);
    
    float ambiance = 0.3f;

    float3 directionalLight;

    float DirectionalLightSoftness = 0.0f;
    float3 DirectionalLightColor = 1.0f;
    float3 DirectionalLightTransform = normalize(float3(cos(time), 1.0f, sin(time)));
    
    if (DirectionalLightSoftness == 0.f)
    {
        directionalLight = EvaluateDirectionalLight(
			diffuseColor, specularColor, pixelNormal, roughness,
			DirectionalLightColor.xyz, DirectionalLightTransform, toEye.xyz);
    }
    else
    {
        directionalLight = EvaluateSoftDirectionalLight(
			diffuseColor, specularColor, pixelNormal, roughness, DirectionalLightSoftness,
			DirectionalLightColor.xyz, DirectionalLightTransform, toEye.xyz);
    }
	
    //float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 radiance = directionalLight + ambiance;


    result.color.rgb = radiance * albedo.rgb;
    result.color.a = albedo.a;
    return result;
}

