cbuffer cameraBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 eyePosition;
};

cbuffer objectBuffer : register(b1)
{
    float4x4 modelToWorldMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float4 worldPosition : POSITION;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    float4x4 objectToScreen = mul(worldToClipMatrix, modelToWorldMatrix);
    output.screenPosition = mul(objectToScreen, input.position);
    output.worldPosition = mul(modelToWorldMatrix, input.position);
    output.normal = mul(modelToWorldMatrix, float4(input.normal, 0.0f));
    output.color = input.color;
    return output;
}