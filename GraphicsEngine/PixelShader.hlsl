#include "Common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = aTexture.Sample(aSampler, input.uv);
    
    return result;
}