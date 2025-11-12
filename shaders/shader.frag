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
    if (texColor.a < 0.1)
    {
        texColor = vec4(0.8, 0.8, 0.8, 1.0);
    }

    // 1. --- LUZ AMBIENTAL FIJA ---
    // Simplemente un color base que ilumina todas las superficies por igual.
    vec3 ambient_color = vec3(1.0, 1.0, 1.0); // Luz blanca
    float ambient_intensity = 0.15;           // Intensidad de relleno (ajústala si es necesario)
    vec3 ambient_component = ambient_color * ambient_intensity;
    // ----------------------------


    // 2. Cálculo de la Luz Difusa (Direccional)
    float diffuse_factor = max(dot(normalize(Normal), normalize(directional_light.direction)), 0.f);
    vec3 diffuse_component = directional_light.color * directional_light.diffuse_intensity * diffuse_factor;
    

    // 3. Combinación de Iluminación y Textura
    // La iluminación total es la suma de la luz ambiental y la luz difusa.
    vec3 total_light = ambient_component + diffuse_component;
    
    // Multiplicamos el color de la textura por la luz total que recibe.
    vec3 final_color = texColor.rgb * total_light;


    color = vec4(final_color, texColor.a);
}
