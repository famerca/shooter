#version 410

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D diffuseTexture;
uniform mat4 model;
uniform mat4 projection;

void main()
{
    // Usar la textura si está disponible, sino usar un color por defecto
    vec4 texColor = texture(diffuseTexture, TexCoord);
    if (texColor.a < 0.1) // Si la textura es transparente o no existe
    {
        texColor = vec4(0.8, 0.8, 0.8, 1.0); // Color gris por defecto
    }
    
    color = texColor;
}