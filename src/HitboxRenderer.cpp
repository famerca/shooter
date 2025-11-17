#include "HitboxRenderer.hpp"

// Incluir OpenGL y utilidades (ajusta esto a tu setup)
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <Jolt/Core/StringTools.h>

namespace Engine {

// --- Constructor ---
HitboxRenderer::HitboxRenderer(std::shared_ptr<Shader> line_shader)
    : mLineShader(std::move(line_shader)), 
      mProjectionMatrix(1.0f), // Inicializar a la matriz identidad
      mViewMatrix(1.0f)
{
    // Establecer el color por defecto en rojo
    SetLineColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); 
}

// --- SetLineColor ---
void HitboxRenderer::SetLineColor(const glm::vec4& color)
{
    mCurrentColor = color;
}

// --- SetViewProjectionMatrix ---
void HitboxRenderer::SetViewProjectionMatrix(const glm::mat4& view, const glm::mat4& proj)
{
    mViewMatrix = view;
    mProjectionMatrix = proj;
}

// --- DrawLine (Implementación Central) ---
void HitboxRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    if (!mLineShader) 
        return;

    // 1. Convertir coordenadas de Jolt a GLM
    // Jolt usa RVec3 (Real Vector 3)
    glm::vec3 from = glm::vec3(inFrom.GetX(), inFrom.GetY(), inFrom.GetZ());
    glm::vec3 to   = glm::vec3(inTo.GetX(), inTo.GetY(), inTo.GetZ());

    // 2. Configurar el estado de OpenGL
    mLineShader->use(); 

    // Opcional: Deshabilita la prueba de profundidad para que las líneas se vean siempre
    glDisable(GL_DEPTH_TEST); 
    
    // 3. Cargar Uniforms (usando tu clase Shader)
    
    // Asumiendo que has calculado la matriz View * Projection
    // Tienes que tener un uniforme en tu shader llamado "view_projection" o similar
    // Si tienes "view" y "projection" separados, necesitas calcular VP aquí o en el shader.
    
    // Ejemplo asumiendo un uniforme llamado "view_projection"

    
    glUniformMatrix4fv(mLineShader->get_uniform_view_id(), 1, GL_FALSE, glm::value_ptr(mViewMatrix));
    glUniformMatrix4fv(mLineShader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(mProjectionMatrix));


    // Cargar el color de la línea (ROJO por defecto)
    // Asumiendo que tienes un uniforme en tu shader llamado "lineColor"
    GLint color_location = mLineShader->get_uniform_line_color_id();
    if (color_location != -1)
    {
        glUniform4fv(color_location, 1, glm::value_ptr(mCurrentColor));
    }


    // 4. Dibujar la línea (implementación simple usando un VBO temporal o VAO/VBO pre-configurado)
    
    // --- Implementación simple (ejemplo, puede ser ineficiente) ---
    // Lo ideal es pre-configurar un VBO y hacer DrawArrays/DrawElements
    
    // Crear un VAO y VBO temporal para la línea (LENTO, SOLO PARA EJEMPLO)
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Datos de la línea (2 vértices)
    float vertices[] = {
        from.x, from.y, from.z,
        to.x,   to.y,   to.z
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo de posición (layout(location = 0) en el Vertex Shader)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Dibujar
    glDrawArrays(GL_LINES, 0, 2); 
    
    // 5. Limpieza
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    
    glEnable(GL_DEPTH_TEST); // Restaurar el estado
}

void HitboxRenderer::DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor, float inHeight)
{
    // Dejar vacío si no quieres renderizar texto de depuración
    // Si lo necesitas, aquí iría tu lógica de renderizado de texto 3D de OpenGL
}

} // namespace Engine