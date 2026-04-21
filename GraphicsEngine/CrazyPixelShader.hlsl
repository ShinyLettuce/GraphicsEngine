#include "Common.hlsli"

struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 direction;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 position;
    float range;
    float3 attenuation;
};

void ComputeDirectionalLight(
    Material m,
    DirectionalLight l,
    float3 normal,
    float3 toEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 specular)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    l.direction = normalize(l.direction);
    
    float3 toLight = -l.direction;
    
    ambient = m.ambient * l.ambient;
    
    float diffuseFactor = dot(toLight, normal);
    
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 reflected = reflect(-toLight, normal);
        
        float specularFactor = pow(max(dot(reflected, toEye), 0.0f), m.specular.w);
        
        diffuse = diffuseFactor * m.diffuse * l.diffuse;
        specular = specularFactor * m.specular * l.specular;
    }
}

void ComputePointLight(
    Material m,
    PointLight l,
    float3 position,
    float3 normal,
    float3 toEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 specular)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 toLight = l.position - position;
    
    float distance = length(toLight);
    if (distance > l.range)
    {
        return;
    }
    
    toLight /= distance;
    
    ambient = m.ambient * l.ambient;
    
    float diffuseFactor = dot(toLight, normal);
    
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 reflected = reflect(-toLight, normal);
        
        float specularFactor = pow(max(dot(reflected, toEye), 0.0f), m.specular.w);
        
        diffuse = diffuseFactor * m.diffuse * l.diffuse;
        specular = specularFactor * m.specular * l.specular;
    }
    
    float attenuation = 1.0f / dot(float3(l.attenuation), float3(1.0f, distance, distance * distance));
    
    diffuse *= attenuation;
    specular *= attenuation;
}

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = input.color;
    
    Material m;
    m.ambient = float4(1.0f, 1.0f, 1.0f, 1.0f);
    m.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
    m.specular = float4(1.0f, 1.0f, 1.0f, 8.0f);
       
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float4 totalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 toEye = normalize(eyePosition - input.worldPosition.xyz);
    
    DirectionalLight directionalLight;
    directionalLight.ambient = float4(0.1f, 0.0f, 0.0f, 1.0f);
    directionalLight.diffuse = float4(1.0f, 0.0f, 0.05f, 1.0f);
    directionalLight.specular = float4(1.0f, 1.0f, 1.0f, 16.0f);
    directionalLight.direction = float3(-1.0f, -1.0f, -1.0f);
    
    ComputeDirectionalLight(
        m, 
        directionalLight,
        input.normal,
        toEye,
        ambient,
        diffuse,
        specular
    );
    
    totalAmbient += ambient;
    totalDiffuse += diffuse;
    totalSpecular += specular;
   
    PointLight pointLight;
    pointLight.ambient = float4(0.0f, 0.0f, 0.01f, 1.0f);
    pointLight.diffuse = float4(0.0f, 0.0f, 1.0f, 1.0f);
    pointLight.specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
    pointLight.position = float3(0.0f, -2.0f, 18.5f);
    pointLight.range = 40.0f;
    pointLight.attenuation = float3(0.0f, 0.0f, 1.0f);
    
    ComputePointLight(
        m,
        pointLight,
        input.worldPosition.xyz,
        input.normal,
        toEye,
        ambient,
        diffuse,
        specular
    );
    
    totalAmbient += ambient;
    totalDiffuse += diffuse;
    totalSpecular += specular;
    
    result.color = totalAmbient + totalDiffuse + totalSpecular;
    
    return result;
}