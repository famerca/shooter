#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
// Nota: Puedes ignorar la matriz 'model' ya que las coordenadas de Jolt 
// ya están en el espacio del mundo.

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
}