#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi/Core/EventListener.h>
#include <functional>

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
    bool Initialize(std::shared_ptr<Engine::Window> window);

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
     * Muestra el menú principal (pausa el juego).
     */
    void ShowMainMenu();

    /**
     * Oculta el menú principal (reanuda el juego).
     */
    void HideMainMenu();

    /**
     * Verifica si el menú principal está visible.
     */
    bool IsMainMenuVisible() const { return main_menu_visible; }

    /**
     * Muestra el menú de pausa (pausa el juego).
     */
    void ShowPauseMenu();

    /**
     * Oculta el menú de pausa (reanuda el juego).
     */
    void HidePauseMenu();

    /**
     * Verifica si el menú de pausa está visible.
     */
    bool IsPauseMenuVisible() const { return pause_menu_visible; }

    /**
     * Establece un callback que se ejecuta cuando el menú se oculta.
     */
    void SetOnMenuHiddenCallback(std::function<void()> callback);

    /**
     * Establece un callback que se ejecuta cuando se solicita reiniciar el juego.
     */
    void SetOnRestartCallback(std::function<void()> callback);

    /**
     * Muestra el contador de vidas.
     */
    void ShowLivesCounter();

    /**
     * Oculta el contador de vidas.
     */
    void HideLivesCounter();

    /**
     * Actualiza el número de vidas mostrado en el contador.
     * @param lives Número de vidas a mostrar
     */
    void UpdateLives(int lives);

    /**
     * Obtiene el contexto de RmlUi (para uso avanzado).
     */
    Rml::Context* GetContext() const { return context; }

    /**
     * Verifica si RmlUi está inicializado.
     */
    bool IsInitialized() const { return initialized; }

private:
    /**
     * EventListener para el botón "START GAME".
     */
    class StartButtonListener : public Rml::EventListener {
    public:
        StartButtonListener(RmlUiInterface* interface) : rmlui_interface(interface) {}
        
        void ProcessEvent(Rml::Event& event) override {
            std::cout << "RmlUiInterface::StartButtonListener: ProcessEvent llamado!" << std::endl;
            std::cout << "RmlUiInterface::StartButtonListener: Tipo de evento: " << static_cast<int>(event.GetId()) << std::endl;
            std::cout << "RmlUiInterface::StartButtonListener: Elemento objetivo: " << event.GetTargetElement()->GetTagName().c_str() << std::endl;
            
            if (event.GetId() == Rml::EventId::Click || event.GetId() == Rml::EventId::Mousedown) {
                std::cout << "RmlUiInterface::StartButtonListener: Botón START GAME presionado!" << std::endl;
                if (rmlui_interface) {
                    std::cout << "RmlUiInterface::StartButtonListener: Ocultando menú principal..." << std::endl;
                    rmlui_interface->HideMainMenu();
                    std::cout << "RmlUiInterface::StartButtonListener: Menú ocultado correctamente" << std::endl;
                } else {
                    std::cerr << "RmlUiInterface::StartButtonListener: ERROR: rmlui_interface es nullptr" << std::endl;
                }
            } else {
                std::cout << "RmlUiInterface::StartButtonListener: Evento recibido pero no es Click/Mousedown (tipo: " 
                          << static_cast<int>(event.GetId()) << ")" << std::endl;
            }
        }
        
        void OnAttach(Rml::Element* element) override {
            std::cout << "RmlUiInterface::StartButtonListener: OnAttach llamado para elemento: " 
                      << element->GetTagName().c_str() << std::endl;
        }
        
        void OnDetach(Rml::Element* element) override {
            std::cout << "RmlUiInterface::StartButtonListener: OnDetach llamado para elemento: " 
                      << element->GetTagName().c_str() << std::endl;
        }

    private:
        RmlUiInterface* rmlui_interface;
    };

    /**
     * EventListener para el botón "CONTINUAR" del menú de pausa.
     */
    class ContinueButtonListener : public Rml::EventListener {
    public:
        ContinueButtonListener(RmlUiInterface* interface) : rmlui_interface(interface) {}
        
        void ProcessEvent(Rml::Event& event) override {
            if (event.GetId() == Rml::EventId::Click) {
                std::cout << "RmlUiInterface: Botón CONTINUAR presionado" << std::endl;
                if (rmlui_interface) {
                    rmlui_interface->HidePauseMenu();
                }
            }
        }

    private:
        RmlUiInterface* rmlui_interface;
    };

    /**
     * EventListener para el botón "REINICIAR JUEGO" del menú de pausa.
     */
    class RestartButtonListener : public Rml::EventListener {
    public:
        RestartButtonListener(RmlUiInterface* interface) : rmlui_interface(interface) {}
        
        void ProcessEvent(Rml::Event& event) override {
            if (event.GetId() == Rml::EventId::Click) {
                std::cout << "RmlUiInterface: Botón REINICIAR JUEGO presionado" << std::endl;
                if (rmlui_interface) {
                    rmlui_interface->HidePauseMenu();
                    if (rmlui_interface->on_restart_callback) {
                        rmlui_interface->on_restart_callback();
                    }
                }
            }
        }

    private:
        RmlUiInterface* rmlui_interface;
    };

private:
    std::shared_ptr<Engine::Window> window;
    Rml::Context* context;
    std::unique_ptr<RenderInterface_GL3> render_interface;
    std::unique_ptr<SystemInterface_GLFW> system_interface;
    bool initialized;
    int window_width;
    int window_height;
    
    // Menú principal
    Rml::ElementDocument* main_menu_document{nullptr};
    bool main_menu_visible{false};
    std::unique_ptr<StartButtonListener> start_button_listener;
    std::function<void()> on_menu_hidden_callback;
    
    // Menú de pausa
    Rml::ElementDocument* pause_menu_document{nullptr};
    bool pause_menu_visible{false};
    std::unique_ptr<ContinueButtonListener> continue_button_listener;
    std::unique_ptr<RestartButtonListener> restart_button_listener;
    std::function<void()> on_restart_callback;
    
    // Contador de vidas
    Rml::ElementDocument* lives_counter_document{nullptr};
    bool lives_counter_visible{false};
    
    // Método helper para cargar el menú de pausa
    bool LoadPauseMenu();
    
    // Método helper para cargar el contador de vidas
    bool LoadLivesCounter();
};

#endif // RMLUI_INTERFACE_HPP

