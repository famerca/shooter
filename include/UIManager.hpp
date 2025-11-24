#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/Event.h>

// Forward declaration
namespace Engine {
    class Window;
}

// Forward declarations
namespace Rml {
    class Context;
    class RenderInterface;
    class SystemInterface;
    class ElementDocument;
    class Element;
}

// RmlUi backend includes
#include "RmlUi_Renderer_GL3.h"
#include "RmlUi_Platform_GLFW.h"

// Namespace para RmlGL3
namespace RmlGL3 {
    bool Initialize(Rml::String* out_message);
    void Shutdown();
}

#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

/**
 * UIManager: Sistema genérico y modular para manejar interfaces de usuario basadas en RmlUi.
 * 
 * 
 * - Sistema de plantillas (templates) con IDs únicos
 * - Eventos manejados con lambdas
 * 
 * 
 */
class UIManager
{
public:
    /**
     * Tipo de callback para eventos de UI
     * Recibe el elemento que disparó el evento y el tipo de evento
     */
    using EventCallback = std::function<void(Rml::Element*, Rml::EventId)>;

    /**
     * Información de una plantilla cargada
     */
    struct TemplateInfo {
        std::string template_id;
        Rml::ElementDocument* document{nullptr};
        bool visible{false};
        std::string file_path;
    };

    UIManager();
    ~UIManager();

    // No copy, no move
    UIManager(const UIManager&) = delete;
    UIManager(UIManager&&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    UIManager& operator=(UIManager&&) = delete;

    /**
     * Inicializa el UIManager usando la ventana del motor.
     * @param window Ventana del motor (debe tener GLFWwindow* inicializado)
     * @param ui_directory Directorio base donde se encuentran los archivos RML (opcional, por defecto "ui")
     * @return true si la inicialización fue exitosa
     */
    bool Initialize(std::shared_ptr<Engine::Window> window, const std::string& ui_directory = "ui");

    /**
     * Cierra y libera todos los recursos.
     */
    void Shutdown();

    /**
     * Actualiza el contexto de RmlUi. Debe llamarse en cada frame antes de renderizar.
     */
    void Update();

    /**
     * Renderiza la interfaz. Debe llamarse después de Update() y antes de swap_buffers().
     */
    void Render();

    /**
     * Carga una plantilla desde un archivo RML.
     * @param template_id ID único para identificar esta plantilla
     * @param file_path Ruta al archivo .rml (relativa al ui_directory o absoluta)
     * @param auto_show Si es true, muestra la plantilla automáticamente al cargar
     * @return true si se cargó correctamente
     */
    bool LoadTemplate(const std::string& template_id, const std::string& file_path, bool auto_show = false);

    /**
     * Descarga una plantilla (libera recursos).
     * @param template_id ID de la plantilla a descargar
     * @return true si se descargó correctamente
     */
    bool UnloadTemplate(const std::string& template_id);

    /**
     * Muestra una plantilla cargada.
     * @param template_id ID de la plantilla a mostrar
     * @return true si se mostró correctamente
     */
    bool ShowTemplate(const std::string& template_id);

    /**
     * Oculta una plantilla cargada.
     * @param template_id ID de la plantilla a ocultar
     * @return true si se ocultó correctamente
     */
    bool HideTemplate(const std::string& template_id);

    /**
     * Verifica si una plantilla está visible.
     * @param template_id ID de la plantilla
     * @return true si está visible
     */
    bool IsTemplateVisible(const std::string& template_id) const;

    /**
     * Verifica si una plantilla está cargada.
     * @param template_id ID de la plantilla
     * @return true si está cargada
     */
    bool IsTemplateLoaded(const std::string& template_id) const;

    /**
     * Registra un callback para un evento en un elemento específico de una plantilla.
     * @param template_id ID de la plantilla
     * @param element_id ID del elemento (button, div, etc.)
     * @param event_type Tipo de evento (Click, Mousedown, etc.)
     * @param callback Función lambda que se ejecutará cuando ocurra el evento
     * @return true si se registró correctamente
     */
    bool RegisterEvent(const std::string& template_id, 
                      const std::string& element_id,
                      Rml::EventId event_type,
                      EventCallback callback);

    /**
     * Actualiza el contenido de texto de un elemento en una plantilla.
     * @param template_id ID de la plantilla
     * @param element_id ID del elemento
     * @param text Nuevo texto a mostrar
     * @return true si se actualizó correctamente
     */
    bool UpdateElementText(const std::string& template_id, const std::string& element_id, const std::string& text);

    /**
     * Obtiene un elemento de una plantilla (para uso avanzado).
     * @param template_id ID de la plantilla
     * @param element_id ID del elemento
     * @return Puntero al elemento o nullptr si no se encuentra
     */
    Rml::Element* GetElement(const std::string& template_id, const std::string& element_id) const;

    /**
     * Obtiene el documento de una plantilla (para uso avanzado).
     * @param template_id ID de la plantilla
     * @return Puntero al documento o nullptr si no se encuentra
     */
    Rml::ElementDocument* GetTemplateDocument(const std::string& template_id) const;

    /**
     * Obtiene el contexto de RmlUi (para uso avanzado).
     */
    Rml::Context* GetContext() const { return context; }

    /**
     * Verifica si el UIManager está inicializado.
     */
    bool IsInitialized() const { return initialized; }

private:
    /**
     * EventListener genérico que ejecuta callbacks lambda.
     */
    class GenericEventListener : public Rml::EventListener {
    public:
        GenericEventListener(EventCallback callback) : callback_(callback) {}
        
        void ProcessEvent(Rml::Event& event) override {
            if (callback_) {
                callback_(event.GetTargetElement(), event.GetId());
            }
        }
        
        void OnAttach(Rml::Element* element) override {}
        void OnDetach(Rml::Element* element) override {}

    private:
        EventCallback callback_;
    };

    /**
     * Información de un evento registrado.
     */
    struct EventRegistration {
        std::string template_id;
        std::string element_id;
        Rml::EventId event_type;
        EventCallback callback;
        std::unique_ptr<GenericEventListener> listener;
    };

    std::shared_ptr<Engine::Window> window;
    Rml::Context* context;
    std::unique_ptr<RenderInterface_GL3> render_interface;
    std::unique_ptr<SystemInterface_GLFW> system_interface;
    bool initialized;
    int window_width;
    int window_height;
    std::string ui_directory;
    
    // Almacenamiento de plantillas
    std::unordered_map<std::string, TemplateInfo> templates;
    
    // Almacenamiento de eventos registrados
    std::vector<EventRegistration> event_registrations;
    
    /**
     * Helper para obtener la ruta completa de un archivo RML.
     */
    std::string GetFullPath(const std::string& file_path) const;
};

#endif // UI_MANAGER_HPP

