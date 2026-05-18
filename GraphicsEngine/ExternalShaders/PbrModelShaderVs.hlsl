#include "../Common.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType result;
	
	float4 vertexObjectPos = input.position;
    //float4 vertexWorldPos = mul(modelToWorldMatrix, vertexObjectPos);
    //float4 vertexViewPos = mul(worldToClipMatrix, vertexWorldPos);
	//float4 vertexProjectionPos = mul(CameraToProjection, vertexViewPos);

    float4 vertexWorldPos = mul(modelToWorldMatrix, vertexObjectPos);
    float4 vertexProjectionPos = mul(worldToClipMatrix, vertexWorldPos);
	
    float3x3 toWorldRotation = (float3x3) modelToWorldMatrix;
	float3 vertexWorldNormal = mul(toWorldRotation, input.normal);
	float3 vertexWorldBinormal = mul(toWorldRotation, input.bitangent);
	float3 vertexWorldTangent = mul(toWorldRotation, input.tangent);
	
	result.screenPosition = vertexProjectionPos;
	result.worldPosition = vertexWorldPos;
	//result.vertexColor0 = input.vertexColor0;
	//result.vertexColor1 = input.vertexColor1;
	//result.vertexColor2 = input.vertexColor2;
	//result.vertexColor3 = input.vertexColor3;

	result.uv = input.uv;
	//result.texCoord0 = input.texCoord0;
	//result.texCoord1 = input.texCoord1;
	//result.texCoord2 = input.texCoord2;
	//result.texCoord3 = input.texCoord3;

	result.normal = vertexWorldNormal;
	result.bitangent = vertexWorldBinormal;
	result.tangent = vertexWorldTangent;
	
	return result;
}