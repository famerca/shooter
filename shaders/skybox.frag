#version 410

in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;
uniform bool use_texture;
uniform vec3 top_color;
uniform vec3 bottom_color;

void main()
{
    if (use_texture)
    {
        color = texture(skybox, TexCoords);
    }
    else
    {
        vec3 normalized = normalize(TexCoords);
        float t = clamp(normalized.y * 0.5 + 0.5, 0.0, 1.0);
        color = vec4(mix(bottom_color, top_color, t), 1.0);
    }
}

