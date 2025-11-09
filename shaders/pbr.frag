#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D texture_albedo;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_ao;

// También soportar versiones numeradas para compatibilidad
uniform sampler2D texture_albedo1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 camPos;

const float PI = 3.14159265359;

// Distribution function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Geometry function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

// Geometry function (Smith)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel-Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{		
    // Cargar texturas PBR (según el plan del usuario)
    // Valores por defecto estándar para PBR
    vec3 albedo = vec3(0.8, 0.8, 0.8);
    float metallic = 0.0;
    float roughness = 0.5;
    float ao = 1.0;
    
    // Intentar cargar texturas (el código C++ debería vincularlas correctamente)
    // El código C++ ahora vincula texturas por defecto si no hay texturas cargadas
    // Intentar primero con versiones sin número, luego con numeradas
    vec3 albedoTex = texture(texture_albedo, TexCoords).rgb;
    if (length(albedoTex) > 0.001) {
        albedo = albedoTex;
    } else {
        albedoTex = texture(texture_albedo1, TexCoords).rgb;
        if (length(albedoTex) > 0.001) {
            albedo = albedoTex;
        }
        // Si no hay textura, mantener el valor por defecto (0.8, 0.8, 0.8)
    }
    
    float metallicTex = texture(texture_metallic, TexCoords).r;
    if (metallicTex > 0.001 || metallicTex < -0.001) {
        metallic = metallicTex;
    } else {
        metallicTex = texture(texture_metallic1, TexCoords).r;
        if (metallicTex > 0.001 || metallicTex < -0.001) metallic = metallicTex;
    }
    
    float roughnessTex = texture(texture_roughness, TexCoords).r;
    if (roughnessTex > 0.001 || roughnessTex < -0.001) {
        roughness = roughnessTex;
    } else {
        roughnessTex = texture(texture_roughness1, TexCoords).r;
        if (roughnessTex > 0.001 || roughnessTex < -0.001) roughness = roughnessTex;
    }
    
    float aoTex = texture(texture_ao, TexCoords).r;
    if (aoTex > 0.001 || aoTex < -0.001) {
        ao = aoTex;
    } else {
        aoTex = texture(texture_ao1, TexCoords).r;
        if (aoTex > 0.001 || aoTex < -0.001) ao = aoTex;
    }

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   
    
    // Luz ambiental estándar para PBR (valores típicos: 0.03-0.1)
    vec3 ambient = vec3(0.05) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping estándar (Reinhard)
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}

