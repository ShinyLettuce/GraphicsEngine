cbuffer FrameData : register(b0)
{
    float3 eyePosition;
    float time;
}

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float4 worldPosition : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

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

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = input.color;
    
    Material m;
    m.ambient = float4(1.0f, 1.0f, 1.0f, 1.0f);
    m.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
    m.specular = float4(1.0f, 1.0f, 1.0f, 8.0f);
    
    DirectionalLight l;
    l.ambient = float4(0.1f, 0.03f, 0.03f, 1.0f);
    l.diffuse = float4(1.0f, 0.0f, 0.3f, 1.0f);
    l.specular = float4(1.0f, 1.0f, 1.0f, 16.0f);
    l.direction = float3(-1.0f, -1.0f, -1.0f);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    ComputeDirectionalLight(
        m, 
        l,
        input.normal,
        normalize(eyePosition - input.worldPosition.xyz),
        ambient,
        diffuse,
        specular
    );
    
    result.color = ambient + diffuse + specular;
    
    return result;
}