#include "../Common.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(PixelInputType input)
{
	PixelOutput result;

    float slopeBlend = smoothstep(0.7f, 1.0f, input.normal.y);
    float heightBlend = smoothstep(-0.05f, 0.25f, input.worldPosition.y);
	
	float2 scaledUV = input.uv;
	
    float3 toEye = normalize(eyePosition - input.worldPosition.xyz);
	
    float4 grassColor = aGrassTexture.Sample(aSampler, input.uv);
    float4 rockColor = aRockTexture.Sample(aSampler, input.uv);
    float4 snowColor = aSnowTexture.Sample(aSampler, input.uv);
	
    float4 albedo = rockColor; //lerp(rockColor, lerp(grassColor, snowColor, heightBlend), slopeBlend);

	//if (albedo.a <= alphaTestThreshold)
	//{
	//	discard;
	//	result.color = float4(0.f, 0.f, 0.f, 0.f);
	//	return result;
	//}
	
    float3 grassNormal = aGrassNormalTexture.Sample(aSampler, input.uv).rgb;
    grassNormal = grassNormal * 2.0 - 1.0;
    grassNormal.z = sqrt(1.0f - (grassNormal.x * grassNormal.x - grassNormal.y * grassNormal.y));

    
    float3 rockNormal = aRockNormalTexture.Sample(aSampler, input.uv).rgb;
    rockNormal.xy = 2.0f * rockNormal.xy - 1.0f;
    rockNormal.z = sqrt(1.0f - saturate(rockNormal.x * rockNormal.x + rockNormal.y * rockNormal.y));
    rockNormal = normalize(rockNormal);
    
    float3 snowNormal = aSnowNormalTexture.Sample(aSampler, input.uv).rgb;
    snowNormal.z = sqrt(1.0f - (snowNormal.x * snowNormal.x - snowNormal.y * snowNormal.y));
    snowNormal = snowNormal * 2.0 - 1.0;
    
    float3 normal = rockNormal; //lerp(rockNormal, lerp(grassNormal, snowNormal, heightBlend), slopeBlend);

	//normal.xy = 2.0f * normal.xy - 1.0f;
	//normal.z = sqrt(1.0f - saturate(normal.x * normal.x + normal.y * normal.y));
	//normal = normalize(normal);

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
    normal = normalize(mul(TBN, normal));

	// TGA Channel Pack. ORM.
	// Metalness, Roughness, Emissive, Emissive Strength (opt).

    float3 grassMaterial = aGrassMaterialTexture.Sample(aSampler, input.uv);
    float3 rockMaterial = aRockMaterialTexture.Sample(aSampler, input.uv);
    float3 snowMaterial = aSnowMaterialTexture.Sample(aSampler, input.uv);
	
    float3 material = rockMaterial;  //lerp(rockMaterial, lerp(grassMaterial, snowMaterial, heightBlend), slopeBlend); // Should be material texture, not grass albedo (SUS!)

    float ambientOcclusion = material.r;
	float metalness = material.b;
    float roughness = material.g;

    //float3 fx = fxTexture.Sample(defaultSampler, scaledUV).rgb;

    //float emissive = fx.r;
	
	float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
	float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1.0f - metalness);

	// ~~~~~~
    float3 AmbientLightColor = float3(1.0f, 1.0f, 1.0f);
	// ~~~~~~
	
    //float3 ambiance = AmbientLightColor.rgb * EvaluateAmbiance(
	//	environmentTexture, pixelNormal, input.normal.xyz, // todo: either disable cubemap or load it using the provided function
	//	toEye, roughness,
	//	ambientOcclusion, diffuseColor, specularColor
	//);
	
	// ~~~~~~
    float3 ambiance = float3(0.05f, 0.05f, 0.1f);
	// ~~~~~~
	
	float3 directionalLight;

	// ~~~~~~
    float DirectionalLightSoftness = 0.0f;
    float3 DirectionalLightColor = 1.0f;
    float3 DirectionalLightDirection = normalize(float3(cos(time * 0.5f), -1.0f, sin(time * 0.5f)));
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
			diffuseColor, specularColor, normal, roughness,
			DirectionalLightColor.xyz, DirectionalLightDirection, -toEye);
    }
    else
    {
        directionalLight = EvaluateSoftDirectionalLight(
			diffuseColor, specularColor, normal, roughness, DirectionalLightSoftness,
			DirectionalLightColor.xyz, DirectionalLightDirection, -toEye);
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
    //if (length(directionalLight) > 0.0f)
    //{
    //    result.color.g = 1.0f;
    //}
	
    //result.color.r = saturate(dot(normal, DirectionalLightDirection) * 0.5f + 0.5f);
	
	//result.color.a = albedo.a;
	return result;
}

