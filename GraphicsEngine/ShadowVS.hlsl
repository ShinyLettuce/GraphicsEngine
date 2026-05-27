#include "Common.hlsli"

ShadowPixelInputType main(VertexInputType input)
{
    ShadowPixelInputType result;
	
    result.position = input.position;
    result.worldPosition = mul(modelToWorldMatrix, input.position);
    result.uv = input.uv;

    return result;
}