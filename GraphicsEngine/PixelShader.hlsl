#include "Common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = aCodeTexture.Sample(aSampler, input.uv);
    
    return result;
}