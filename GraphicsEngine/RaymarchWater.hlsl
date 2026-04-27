#include "Common.hlsli"

#define MAX_RAYMARCH_STEP_COUNT 128
#define MIN_SURFACE_DISTANCE 0.01f
#define MAX_DISTANCE 128

float2 GetWaterUV(float3 p)
{
    return (p.xz * 0.05f) + float2(1.0f, 0.0f) * time * 0.033f;
}

float GetDistanceToCylinder(float3 p, float r, float h)
{
    float2 d = abs(float2(length(p.yz), p.x)) - float2(r, h);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float GetDistanceToSphere(float3 p, float radius)
{
    return length(p) - radius;
}

float GetDistanceToWater(float3 p)
{
    float d = p.y;
    float h = aTexture.Sample(aSampler, GetWaterUV(p)).r * 1.5f;
    return d + h;
}

float GetDistanceToScene(float3 p)
{
    float d1 = GetDistanceToWater(p);
    float d2 = GetDistanceToSphere(p, 8.0f);
    return max(d1, d2);
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
    float2 offset = float2(0.05f, 0.0f);
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
    
    float d = Raymarch(rayOrigin, rayDirection);
    
    float3 p = rayOrigin + rayDirection * d;
    
    float3 normal = GetNormal(p);
    
    if (d < MAX_DISTANCE)
    {
        float value = aTexture.Sample(aSampler, GetWaterUV(p));
        float3 lightPosition = float3(cos(time) * 5.0f, 3.0f, sin(time) * 5.0f);
        float distanceToLight = distance(lightPosition, p);
        float lightByDistance = 1.0f / (distanceToLight * distanceToLight);
        float lightByAngle = saturate(dot(normal, normalize(lightPosition - p)) * 0.5f + 0.5f);
        result.color.rgb = (lightByDistance * 2.0f * lightByAngle);
    }
    else
    {
        discard;
    }

    return result;
}