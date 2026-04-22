#include "Common.hlsli"

#define MAX_RAYMARCH_STEP_COUNT 64
#define MIN_SURFACE_DISTANCE 0.01f
#define MAX_DISTANCE 50

float GetDistanceToScene(float3 p)
{
    float d = p.y + sin(p.x + time);
    return d;
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
    
    if (distance < MAX_DISTANCE)
    {
        result.color.rgb = lerp(GetNormal(p), float3(0.05f, 0.05f, 0.05f), distance / MAX_DISTANCE);
        result.color.a = 1.0f;
    }
    else
    {
        discard;
    }

    return result;
}