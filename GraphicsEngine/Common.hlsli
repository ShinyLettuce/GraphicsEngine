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
Texture2D aGrassTexture : register(t1);
Texture2D aGrassNormalTexture : register(t2);
Texture2D aRockTexture : register(t3);
Texture2D aRockNormalTexture : register(t4);
Texture2D aSnowTexture : register(t5);
Texture2D aSnowNormalTexture : register(t6);

SamplerState aSampler : register(s0);

struct VertexInputType
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float4 worldPosition : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};
