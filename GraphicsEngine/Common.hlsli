cbuffer cameraBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 eyePosition;
};

cbuffer objectBuffer : register(b1)
{
    float4x4 modelToWorldMatrix;
};

cbuffer frameBuffer : register(b1)
{
    float time;
}

Texture2D aTexture : register(t0);
Texture2D aCodeTexture : register(t1);

SamplerState aSampler : register(s0);

struct VertexInputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
    float4 worldPosition : POSITION;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};