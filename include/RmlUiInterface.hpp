#pragma once

#include <memory>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include "Window.hpp"

// Forward declarations
namespace Rml {
    class Context;
    class RenderInterface;
    class SystemInterface;
}

// RmlUi backend includes (paths relativos desde Backends/)
#include "RmlUi_Renderer_GL3.h"
#include "RmlUi_Platform_GLFW.h"

// Namespace para RmlGL3 (declaración forward, sin default argument para evitar redefinición)
namespace RmlGL3 {
    bool Initialize(Rml::String* out_message);
    void Shutdown();
}

#ifndef RMLUI_INTERFACE_HPP
#define RMLUI_INTERFACE_HPP

/**
 * Clase wrapper que encapsula RmlUi usando los componentes existentes del motor.
 * Utiliza el RenderInterface_GL3 y SystemInterface_GLFW de RmlUi.
 */
class RmlUiInterface
{
public:
    RmlUiInterface();
    ~RmlUiInterface();

    // No copy, no move
    RmlUiInterface(const RmlUiInterface&) = delete;
    RmlUiInterface(RmlUiInterface&&) = delete;
    RmlUiInterface& operator=(const RmlUiInterface&) = delete;
    RmlUiInterface& operator=(RmlUiInterface&&) = delete;

    /**
     * Inicializa RmlUi usando la ventana existente.
     * @param window Ventana del motor (debe tener GLFWwindow* inicializado)
     * @return true si la inicialización fue exitosa
     */
    bool Initialize(std::shared_ptr<Window> window);

    /**
     * Cierra y libera todos los recursos de RmlUi.
     */
    void Shutdown();

    /**
     * Actualiza el contexto de RmlUi. Debe llamarse en cada frame antes de renderizar.
     */
    void Update();

    /**
     * Renderiza la interfaz de RmlUi. Debe llamarse después de Update() y antes de swap_buffers().
     */
    void Render();

    /**
     * Procesa eventos de entrada de GLFW y los envía a RmlUi.
     * Debe llamarse cuando se procesen eventos de teclado/mouse.
     */
    void ProcessInput(GLFWwindow* window);

    /**
     * Carga un documento RML desde un archivo.
     * @param document_path Ruta al archivo .rml
     * @return true si se cargó correctamente
     */
    bool LoadDocument(const std::string& document_path);

    /**
     * Obtiene el contexto de RmlUi (para uso avanzado).
     */
    Rml::Context* GetContext() const { return context; }

    /**
     * Verifica si RmlUi está inicializado.
     */
    bool IsInitialized() const { return initialized; }

private:
    std::shared_ptr<Window> window;
    Rml::Context* context;
    std::unique_ptr<RenderInterface_GL3> render_interface;
    std::unique_ptr<SystemInterface_GLFW> system_interface;
    bool initialized;
    int window_width;
    int window_height;
};

#endif // RMLUI_INTERFACE_HPP

