struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = float4(input.normal, 1.0f);
    return result;
}