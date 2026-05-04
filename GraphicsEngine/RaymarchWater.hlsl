#include "Common.hlsli"

#define MAX_RAYMARCH_SURFACE_STEP_COUNT 64
#define MAX_RAYMARCH_VOLUME_STEP_COUNT 256
#define MAX_RAYMARCH_SHADOW_STEP_COUNT 64
#define MIN_SURFACE_DISTANCE 0.01
#define MAX_SURFACE_DISTANCE 100
#define VOLUME_MARCH_DISTANCE 0.1

#define LIGHT_POSITION float3(cos(time * 0.5f) * 4.0f, 8.0f, sin(time * 0.5f) * 4.0f)

float SmoothUnion(float a, float b, float k)
{
    k *= 2.0;
    float x = (b - a) / k;
    float g = 0.5 * (x + sqrt(x * x + 1.0));
    return b - k * g;
}

float Hash(float n)
{
    return frac(sin(n) * 753.5453123);
}

float LightIntensity(float intensity, float distance)
{
    return intensity / (distance * distance);
}

float BeerLambert(float absorptionCoefficient, float distanceTraveled)
{
    return exp(-absorptionCoefficient * distanceTraveled);
}

float GetDistanceToAxisAlignedPlane(float3 p)
{
    return p.y;
}

float GetDistanceToCube(float3 p, float3 r)
{
    float3 q = abs(p) - r;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float GetDistanceToSphere(float3 p, float radius)
{
    float d = length(p) - radius;
    return d;
}

float GetDistanceToTorus(float3 p, float2 r)
{
    float2 q = float2(length(p.xz) - r.y, p.y);
    return length(q) - r.x;
}

float GetDistanceToVolume(float3 p)
{
    float d = GetDistanceToCube(p - float3(-8.0f, 8.0f, 0.0f), 8.0f);
    d = SmoothUnion(d, GetDistanceToAxisAlignedPlane(p), 3.0f);
    d = max(d, -GetDistanceToAxisAlignedPlane(p));
    return d;
}

float GetDistanceToSurface(float3 p)
{
    float d = SmoothUnion(
        GetDistanceToTorus(p - float3(4.0f * sin(time * 0.5f), 5.0f, 1.0f), float2(1.0f, 3.0f)),
        GetDistanceToCube(p - float3(-8.0f, 3.0f, 1.0f), 2.4f),
        1.0f);
    d = min(d, GetDistanceToSphere(p - float3(-4.0f, 10.0f + 2.0f * sin(time * 0.3f), -5.0f), 1.5f));
    d = min(d, GetDistanceToAxisAlignedPlane(p));
    return d;
}

float RaymarchSurface(float3 rayOrigin, float3 rayDirection)
{
    float rayDistance = 0.0f;
    for (int i = 0; i < MAX_RAYMARCH_SURFACE_STEP_COUNT; ++i)
    {
        float distanceToSurface = GetDistanceToSurface(rayOrigin + rayDirection * rayDistance);
        
        rayDistance += distanceToSurface;
        
        if (distanceToSurface < MIN_SURFACE_DISTANCE || distanceToSurface > MAX_SURFACE_DISTANCE)
        {
            break;
        }
    }
    
    return rayDistance;
}

float RaymarchShadow(float3 rayOrigin, float3 rayDirection, float maxRayDistance, float harshness)
{
    float light = 1.0f;
    float rayDistance = MIN_SURFACE_DISTANCE + 0.1f;
    for (int i = 0; i < MAX_RAYMARCH_SHADOW_STEP_COUNT && rayDistance < maxRayDistance; ++i)
    {
        float distanceToSurface = GetDistanceToSurface(rayOrigin + rayDirection * rayDistance);
        if (distanceToSurface < MIN_SURFACE_DISTANCE)
        {
            return 0.0f;
        }

        light = min(light, harshness * distanceToSurface / rayDistance);

        rayDistance += distanceToSurface;
    }
    
    return light;
}

float3 RaymarchVolume(float3 rayOrigin, float3 rayDirection, float maxRayDistance)
{
    float distanceToVolume = GetDistanceToVolume(rayOrigin);  // Only works because there is only 1 volume in the scene;
    maxRayDistance -= distanceToVolume;
    rayOrigin = rayOrigin + rayDirection * distanceToVolume;
    
    float3 accumulatedColor = 0.0f;

    float transmittance = 1.0f;
    
    float rayDistance = 0.0f;
    for (int i = 0; i < MAX_RAYMARCH_VOLUME_STEP_COUNT && rayDistance < maxRayDistance; ++i)
    {
        rayDistance += VOLUME_MARCH_DISTANCE;
        
        float3 rayPoint = rayOrigin + rayDirection * rayDistance;
        float distanceToSurface = GetDistanceToVolume(rayPoint);
        
        if (distanceToSurface < MIN_SURFACE_DISTANCE)
        {
            float previousTransmittance = transmittance;
            transmittance *= BeerLambert(0.05f, VOLUME_MARCH_DISTANCE);
            
            float absorption = previousTransmittance - transmittance;
            
            float distanceToLight = length(LIGHT_POSITION - rayPoint);
            float light = LightIntensity(10.0f, distanceToLight);
            float shadow = RaymarchShadow(rayPoint, normalize(LIGHT_POSITION - rayPoint), distanceToLight, 24.0f);
            accumulatedColor += absorption * light * shadow;
        }
    }
    
    return accumulatedColor;
}

float Light(float3 p, float3 normal)
{
    float lightIntensity = 200.0f;
    float3 ambientColor = float3(0.01f, 0.01f, 0.01f);  
    
    float light = saturate(dot(normal, LIGHT_POSITION - p));
    
    float distanceToLight = length(LIGHT_POSITION - p);
    float shadow = RaymarchShadow(p, normalize(LIGHT_POSITION - p), distanceToLight, 24.0f);
    
    return light * shadow * LightIntensity(lightIntensity, length(LIGHT_POSITION - p)) + ambientColor;
}

float3 Normal(float3 p)
{
    float d = GetDistanceToSurface(p);
    float2 e = float2(0.01f, 0.0f);
    
    float3 normal = d - float3(
        GetDistanceToSurface(p - e.xyy),
        GetDistanceToSurface(p - e.yxy),
        GetDistanceToSurface(p - e.yyx)
    );

    return normal;
}

PixelOutput main(PixelInputType input)
{    
    PixelOutput output;
    
    float3 rayOrigin = eyePosition;
    float3 rayDirection = normalize(input.worldPosition.xyz - eyePosition);
    
    float distanceToSurface = RaymarchSurface(rayOrigin, rayDirection);
    float3 volumeColor = RaymarchVolume(rayOrigin, rayDirection, distanceToSurface);
    
    float3 p = rayOrigin + rayDirection * distanceToSurface;
    
    float3 normal = Normal(p);
    float3 light = Light(p, normal);

    if (distanceToSurface < MAX_SURFACE_DISTANCE)
    {
        output.color.rgb = light + volumeColor;
    }
    else
    {
        output.color.rgb = volumeColor;
    }
      
    return output;
}