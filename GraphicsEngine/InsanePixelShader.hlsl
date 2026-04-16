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
    result.color.rgb = sin(input.position.rgb * 0.1f + 2.15f) % 0.8f;
    result.color.b = result.color.r;
    result.color.rg = 0;
    result.color.b = max(result.color.b, sin((input.position.y + input.position.x + 100.f) * 0.2f) * 0.1f);
    result.color.b = max(result.color.b, sin((input.position.y - input.position.x - 100.f) * 0.2f) * 0.1f);
    result.color.a = 1.0f;

    return result;
}