#version 410

in vec4 vertex_color;

out vec4 color;
uniform mat4 model;
uniform mat4 projection;

void main()
{
    color = vertex_color;
}