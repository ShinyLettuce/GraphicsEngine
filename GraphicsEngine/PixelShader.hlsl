#include "Common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color.rg = input.uv;
    result.color.ba = float2(0.0f, 0.0f);
    return result;
}