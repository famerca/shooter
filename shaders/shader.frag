#version 410

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D diffuseTexture;

struct DirectionalLight
{
    vec3 color;
    vec3 direction;
    float diffuse_intensity;
};

uniform DirectionalLight directional_light;

void main()
{
    // Usar la textura si está disponible, sino usar un color por defecto
    vec4 texColor = texture(diffuseTexture, TexCoord);
    if (texColor.a < 0.1) // Si la textura es transparente o no existe
    {
        texColor = vec4(0.8, 0.8, 0.8, 1.0); // Color gris por defecto
    }

    float diffuse_factor = max(dot(normalize(Normal), normalize(directional_light.direction)), 0.f);
    vec4 diffuse_color = vec4(directional_light.color, 1.f) * directional_light.diffuse_intensity * diffuse_factor;

    
    
    color = texColor * diffuse_color;
}