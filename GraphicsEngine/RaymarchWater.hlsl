#include "Common.hlsli"

#define MAX_RAYMARCH_STEP_COUNT 128
#define MIN_SURFACE_DISTANCE 0.001f
#define MAX_DISTANCE 50

float2 GetWaterUV(float3 p)
{
    return (p.xz * 0.01f) + float2(1.0f, 0.0f) * time * 0.01f;
}

float GetDistanceToCylinder(float3 p, float r, float h)
{
    float2 d = abs(float2(length(p.yz), p.x)) - float2(r, h);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float GetDistanceToWater(float3 p)
{
    float d = p.y - 0.5f;
    float h = 0.0f;
    //h += aTexture.Sample(aSampler, GetWaterUV(p, float2(1.0f, 0.0f))).r * 3.0f;
    h += aTexture.Sample(aSampler, GetWaterUV(p)).r * 1.0f;
    //h += aTexture.Sample(aSampler, GetWaterUV(p, normalize(float2(1.0f, 1.0f)))).r * 3.0f;
    return d + h;
}

float GetDistanceToScene(float3 p)
{
    float d1 = GetDistanceToWater(p);
    float d2 = GetDistanceToCylinder(p, 2.5f, 4.0f);
    float d3 = GetDistanceToCylinder(p - float3(-3.5f, 0.0f, 0.0f), 1.2f, 2.0f);
    return max(d1, min(d2, d3));
}

float Raymarch(float3 rayOrigin, float3 rayDirection)
{
    float distanceFromOrigin = 0.0f;
    float distanceToScene;
    
    for (int i = 0; i < MAX_RAYMARCH_STEP_COUNT; ++i)
    {
        float3 p = rayOrigin + rayDirection * distanceFromOrigin;
        distanceToScene = GetDistanceToScene(p);
        distanceFromOrigin += distanceToScene;
        if (distanceToScene < MIN_SURFACE_DISTANCE || distanceFromOrigin > MAX_DISTANCE)
        {
            break;
        }
    }
    
    return distanceFromOrigin;
}

float3 GetNormal(float3 p)
{
    float2 offset = float2(0.01f, 0.0f);
    float3 normal = GetDistanceToScene(p) - float3(
            GetDistanceToScene(p-offset.xyy),
            GetDistanceToScene(p-offset.yxy),
            GetDistanceToScene(p-offset.yyx));
    return normalize(normal);
}

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    float3 rayOrigin = eyePosition;
    float3 rayDirection = normalize(input.worldPosition.xyz - eyePosition);
    
    float distance = Raymarch(rayOrigin, rayDirection);
    
    float3 p = rayOrigin + rayDirection * distance;
    
    float3 normal = GetNormal(p);
    
    if (distance < MAX_DISTANCE)
    {
        //result.color.rg = abs(p.xz) % 1.0f;
        //result.color.a = 1.0f;
        
        float value = saturate((1.0f - dot(normal, float3(0.0f, 1.0f, 0.0f))) / 0.2f);
        float3 color = lerp(float3(171, 240, 255) / 255, float3(34, 57, 99) / 255, value);
        result.color.rgb = normal; //aTexture.Sample(aSampler, GetWaterUV(p, float2(1.0f, 0.0f)));
    }
    else
    {
        discard;
    }

    return result;
}