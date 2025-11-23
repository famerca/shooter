#version 410

layout (location = 0) in vec3 pos;

out vec3 texture_coordinates;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Invertir Z para corregir la orientación del skybox
    texture_coordinates = vec3(pos.x, pos.y, -pos.z);
    
    // Renderizar el skybox en el far plane usando xyww
    vec4 clipPos = projection * view * vec4(pos, 1.0);
    gl_Position = clipPos.xyww; // Esto asegura que z siempre sea 1.0 (far plane)
}