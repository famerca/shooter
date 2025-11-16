#version 330 core
out vec4 FragColor;

uniform vec4 lineColor; // Lo usaremos para el color dinámico (rojo)

void main()
{
    FragColor = lineColor;
}