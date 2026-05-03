#include "Common.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    float4x4 objectToScreen = mul(worldToClipMatrix, modelToWorldMatrix);
    //input.position.y += aTexture.Sample(aSampler, input.uv).r * 10.0f;
    output.screenPosition = mul(objectToScreen, input.position);
    output.worldPosition = mul(modelToWorldMatrix, input.position);
    output.normal = mul(modelToWorldMatrix, float4(input.normal, 0.0f));
    output.uv = input.uv;
    output.color = input.color;
    return output;
}