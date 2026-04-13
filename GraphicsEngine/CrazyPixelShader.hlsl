struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    float factor = 0.9f;
    float pi = 3.1415927f;
    result.color = saturate(sin(input.position.x * factor)) + saturate(sin(input.position.x * factor + pi));
    result.color.r -= sin(input.position.y * factor);
    result.color.g -= sin(input.position.y * factor + pi);
    return result;
}