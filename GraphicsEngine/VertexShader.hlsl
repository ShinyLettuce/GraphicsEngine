cbuffer frameBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
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
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    float4x4 objectToScreen = mul(worldToClipMatrix, modelToWorldMatrix);
    output.position = mul(objectToScreen, input.position);
    output.normal = input.normal;
    output.color = input.color;
    return output;
}