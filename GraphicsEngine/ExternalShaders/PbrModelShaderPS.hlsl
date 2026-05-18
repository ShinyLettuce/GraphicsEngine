#include "../Common.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(PixelInputType input)
{
	PixelOutput result;

	float2 scaledUV = input.uv;
	
    float3 toEye = normalize(eyePosition - input.worldPosition.xyz);
    float4 albedo = aGrassTexture.Sample(aSampler, scaledUV).rgba;

	//if (albedo.a <= alphaTestThreshold)
	//{
	//	discard;
	//	result.color = float4(0.f, 0.f, 0.f, 0.f);
	//	return result;
	//}

    float3 normal = aGrassNormalTexture.Sample(aSampler, scaledUV).xyy;

	normal.xy = 2.0f * normal.xy - 1.0f;
	normal.z = sqrt(1.0f - saturate(normal.x * normal.x + normal.y * normal.y));
	normal = normalize(normal);

	float3x3 TBN = float3x3(
		normalize(input.tangent.xyz),
		normalize(-input.bitangent.xyz),
		normalize(input.normal.xyz));

	// Can save an instruction here by instead doing
	// normalize(mul(normal, TBN)); It works because
	// TBN is a 3x3 and therefore TBN^T is the same
	// as TBN^-1. However, it is considered good form
	// to do this.
	TBN = transpose(TBN);
	float3 pixelNormal = normalize(mul(TBN, normal));

	// TGA Channel Pack. ORM.
	// Metalness, Roughness, Emissive, Emissive Strength (opt).

    float3 material = aGrassTexture.Sample(aSampler, scaledUV).rgb; // Should be material texture, not grass albedo (SUS!)

    float ambientOcclusion = material.r;
	float metalness = material.b;
    float roughness = material.g;

    //float3 fx = fxTexture.Sample(defaultSampler, scaledUV).rgb;

    //float emissive = fx.r;
	
	float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
	float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

	// ~~~~~~
    float3 AmbientLightColor = float3(1.0f, 1.0f, 1.0f);
	// ~~~~~~
	
    //float3 ambiance = AmbientLightColor.rgb * EvaluateAmbiance(
	//	environmentTexture, pixelNormal, input.normal.xyz, // todo: either disable cubemap or load it using the provided function
	//	toEye, roughness,
	//	ambientOcclusion, diffuseColor, specularColor
	//);
	
	// ~~~~~~
    float3 ambiance = float3(0.2f, 0.2f, 0.2f);
	// ~~~~~~
	
	float3 directionalLight;

	// ~~~~~~
    float DirectionalLightSoftness = 0.0f;
    float3 DirectionalLightColor = 1.0f;
    float3 DirectionalLightDirection = normalize(float3(1.0f, -1.0f, 1.0f));
	// ~~~~~~
	
	//if (DirectionalLightSoftness == 0.f)
	//{
	//	directionalLight = EvaluateDirectionalLight(
	//		diffuseColor, specularColor, pixelNormal, roughness,
	//		DirectionalLightColor.xyz, DirectionalLightTransform._m02_m12_m22, toEye.xyz);
	//}
	//else 
	//{
	//	directionalLight = EvaluateSoftDirectionalLight(
	//		diffuseColor, specularColor, pixelNormal, roughness, DirectionalLightSoftness,
	//		DirectionalLightColor.xyz, DirectionalLightTransform._m02_m12_m22, toEye.xyz);
	//}
	
    if (DirectionalLightSoftness == 0.f)
    {
        directionalLight = EvaluateDirectionalLight(
			diffuseColor, specularColor, pixelNormal, roughness,
			DirectionalLightColor.xyz, DirectionalLightDirection, toEye.xyz);
    }
    else
    {
        directionalLight = EvaluateSoftDirectionalLight(
			diffuseColor, specularColor, pixelNormal, roughness, DirectionalLightSoftness,
			DirectionalLightColor.xyz, DirectionalLightDirection, toEye.xyz);
    }
	
	//float3 pointLights = 0; // <- The sum of all point lights.
	//for(unsigned int p = 0; p < NumberOfLights; p++)
	//{
	//	if (myPointLights[p].Radius == 0.f)
	//	{
	//		pointLights += EvaluatePointLight(
	//			diffuseColor, specularColor, pixelNormal, roughness,
	//			myPointLights[p].Color.rgb, myPointLights[p].Range, myPointLights[p].Position.xyz,
	//			toEye.xyz, input.worldPosition.xyz);
	//	}
	//	else
	//	{
	//		pointLights += EvaluateSoftAreaLight(
	//			diffuseColor, specularColor, pixelNormal, roughness,
	//			myPointLights[p].Color.rgb, myPointLights[p].Radius, myPointLights[p].Range, myPointLights[p].Position.xyz,
	//			toEye.xyz, input.worldPosition.xyz);
	//	}
	//}
	
	//float3 emissiveAlbedo = albedo.rgb * emissive;
	//float3 radiance = directionalLight + ambiance + pointLights + emissiveAlbedo;
	float3 radiance = directionalLight + ambiance;

    result.color.rgb = radiance;
	//result.color.a = albedo.a;
	return result;
}

