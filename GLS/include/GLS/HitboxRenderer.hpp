// OpenGLHitboxRenderer.hpp
#pragma once

// Asegúrate de incluir los encabezados de Jolt y tu clase Shader
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <glm/glm.hpp> 
#include <memory>
#include "Shader.hpp" // Tu clase Shader

#ifndef HITBOX_RENDERER
#define HITBOX_RENDERER
// Define el namespace si usas uno
namespace Engine {


class HitboxRenderer : public JPH::DebugRendererSimple
{
public:
    // Constructor. Recibe la instancia de tu shader de líneas.
    explicit HitboxRenderer(std::shared_ptr<Shader> line_shader);

    // Destructor
    virtual ~HitboxRenderer() = default;

    // Método principal de Jolt que debes implementar
    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;

    virtual void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor = JPH::Color::sWhite, float inHeight = 0.5f) override;

    // Nuevo método para establecer el color de las líneas de depuración
    void SetLineColor(const glm::vec4& color);

    // Método para actualizar las matrices de vista y proyección
    void SetViewProjectionMatrix(const glm::mat4& view, const glm::mat4& proj);

private:
    // Tu shader de líneas (propiedad de esta clase)
    std::shared_ptr<Shader> mLineShader;

    // Color que se usará para dibujar las líneas
    glm::vec4 mCurrentColor;

    // Matriz de Vista-Proyección para transformar las coordenadas del mundo
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
};

} // namespace Engine

#endif // HITBOX_RENDERER