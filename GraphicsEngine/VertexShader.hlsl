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
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.position = mul(worldToClipMatrix, input.position);
    output.color = input.color;
    return output;
}