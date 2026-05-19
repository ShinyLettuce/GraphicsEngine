#include "../Common.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;

    float2 scaledUV = input.uv;
	
    float3 toEye = normalize(eyePosition - input.worldPosition.xyz);
    float4 albedo = aRockTexture.Sample(aSampler, scaledUV);

    //if (albedo.a <= alphaTestThreshold)
    //{
    //    discard;
    //    result.color = float4(0.f, 0.f, 0.f, 0.f);
    //    return result;
    //}

    float3 normal = aRockNormalTexture.Sample(aSampler, scaledUV).xyy;

    normal.xy = 2.0f * normal.xy - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = normalize(normal);

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

    float3 material = aRockMaterialTexture.Sample(aSampler, scaledUV).rgb;

    float ambientOcclusion = material.r;
    float metalness = material.b;
    float roughness = material.g;

    //float3 fx = fxTexture.Sample(defaultSampler, scaledUV).rgb;

    //float emissive = fx.r;
	
    float3 specularColor = 1.0f; //lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = 0.0f; //lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

    //float3 ambiance = AmbientLightColor.rgb * EvaluateAmbiance(
	//	environmentTexture, pixelNormal, input.normal.xyz,
	//	toEye, roughness,
	//	ambientOcclusion, diffuseColor, specularColor
	//);
    
    float ambiance = 0.01f;

    float3 directionalLight;

    float DirectionalLightSoftness = 0.0f;
    float3 DirectionalLightColor = 1.0f;
    float3 DirectionalLightTransform = normalize(float3(1.0f, -1.0f, 1.0f));
    
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


    result.color.rgb = (float3) radiance;
    result.color.a = albedo.a;
    return result;
}

