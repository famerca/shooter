#include "RmlUiInterface.hpp"
#include "Window.hpp"
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Element.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

RmlUiInterface::RmlUiInterface()
    : context(nullptr)
    , render_interface(nullptr)
    , system_interface(nullptr)
    , initialized(false)
    , window_width(0)
    , window_height(0)
    , main_menu_document(nullptr)
    , main_menu_visible(false)
    , start_button_listener(std::make_unique<StartButtonListener>(this))
    , pause_menu_document(nullptr)
    , pause_menu_visible(false)
    , continue_button_listener(std::make_unique<ContinueButtonListener>(this))
    , restart_button_listener(std::make_unique<RestartButtonListener>(this))
{
}

RmlUiInterface::~RmlUiInterface()
{
    Shutdown();
}

bool RmlUiInterface::Initialize(std::shared_ptr<Engine::Window> window)
{
    if (initialized)
    {
        std::cerr << "RmlUiInterface: Ya está inicializado" << std::endl;
        return false;
    }

    if (!window || !window->getGLFWWindow())
    {
        std::cerr << "RmlUiInterface: Ventana inválida" << std::endl;
        return false;
    }

    this->window = window;
    GLFWwindow* glfw_window = window->getGLFWWindow();

    // Obtener dimensiones de la ventana
    glfwGetFramebufferSize(glfw_window, &window_width, &window_height);

    // Inicializar RmlGL3 (carga funciones de OpenGL)
    // Nota: Como ya tenemos GLEW, esto puede no ser necesario, pero RmlUi lo requiere
    Rml::String renderer_message;
    if (!RmlGL3::Initialize(&renderer_message))
    {
        std::cerr << "RmlUiInterface: Error al inicializar RmlGL3" << std::endl;
        return false;
    }
    std::cout << "RmlUiInterface: " << renderer_message << std::endl;

    // Crear las interfaces de renderizado y sistema
    render_interface = std::make_unique<RenderInterface_GL3>();
    if (!*render_interface)
    {
        std::cerr << "RmlUiInterface: Error al crear RenderInterface" << std::endl;
        RmlGL3::Shutdown();
        return false;
    }

    system_interface = std::make_unique<SystemInterface_GLFW>();
    system_interface->SetWindow(glfw_window);

    // Configurar viewport del renderer
    render_interface->SetViewport(window_width, window_height);

    // Instalar las interfaces en RmlUi
    Rml::SetRenderInterface(render_interface.get());
    Rml::SetSystemInterface(system_interface.get());

    // Inicializar RmlUi
    if (!Rml::Initialise())
    {
        std::cerr << "RmlUiInterface: Error al inicializar RmlUi" << std::endl;
        render_interface.reset();
        system_interface.reset();
        RmlGL3::Shutdown();
        return false;
    }

    // Crear el contexto principal
    context = Rml::CreateContext("main", Rml::Vector2i(window_width, window_height));
    if (!context)
    {
        std::cerr << "RmlUiInterface: Error al crear contexto" << std::endl;
        Rml::Shutdown();
        render_interface.reset();
        system_interface.reset();
        RmlGL3::Shutdown();
        return false;
    }

    // Inicializar el debugger (opcional, presiona F8 para activarlo)
    Rml::Debugger::Initialise(context);

    // Cargar fuentes (necesario antes de cargar documentos)
    // Intentar cargar desde diferentes ubicaciones
    fs::path font_paths[] = {
        fs::path(__FILE__).parent_path().parent_path() / "ui" / "fonts" / "LatoLatin-Regular.ttf",
        fs::path(__FILE__).parent_path().parent_path() / "build" / "third_party" / "RmlUi" / "Samples" / "assets" / "LatoLatin-Regular.ttf"
    };

    bool font_loaded = false;
    for (const auto& font_path : font_paths)
    {
        if (fs::exists(font_path))
        {
            if (Rml::LoadFontFace(font_path.string()))
            {
                std::cout << "RmlUiInterface: Fuente cargada: " << font_path << std::endl;
                font_loaded = true;
                break;
            }
        }
    }

    if (!font_loaded)
    {
        std::cerr << "RmlUiInterface: Advertencia: No se pudo cargar ninguna fuente. Los documentos pueden no renderizarse correctamente." << std::endl;
    }

    initialized = true;
    std::cout << "RmlUiInterface: Inicializado correctamente" << std::endl;

    return true;
}

void RmlUiInterface::Shutdown()
{
    if (!initialized)
        return;

    if (context)
    {
        Rml::Debugger::Shutdown();
        // El contexto se destruye automáticamente cuando se llama a Rml::Shutdown()
        // o se puede destruir explícitamente con Rml::RemoveContext()
        Rml::RemoveContext("main");
        context = nullptr;
    }

    Rml::Shutdown();
    render_interface.reset();
    system_interface.reset();
    RmlGL3::Shutdown();

    initialized = false;
    std::cout << "RmlUiInterface: Cerrado correctamente" << std::endl;
}

void RmlUiInterface::Update()
{
    if (!initialized || !context)
        return;

    // Actualizar el tamaño del viewport si la ventana cambió de tamaño
    if (window && window->getGLFWWindow())
    {
        int current_width, current_height;
        glfwGetFramebufferSize(window->getGLFWWindow(), &current_width, &current_height);
        
        if (current_width != window_width || current_height != window_height)
        {
            window_width = current_width;
            window_height = current_height;
            render_interface->SetViewport(window_width, window_height);
            context->SetDimensions(Rml::Vector2i(window_width, window_height));
        }
    }

    // Actualizar el contexto (procesa animaciones, eventos, etc.)
    context->Update();
}

void RmlUiInterface::Render()
{
    if (!initialized || !context || !render_interface)
        return;

    // Preparar el renderer para recibir comandos de RmlUi
    render_interface->BeginFrame();

    // Renderizar el contexto
    context->Render();

    // Finalizar el frame
    render_interface->EndFrame();
}

void RmlUiInterface::ProcessInput(GLFWwindow* /*glfw_window*/)
{
    if (!initialized || !context)
        return;

    // Esta función debe ser llamada desde los callbacks de GLFW
    // Por ahora, la dejamos vacía ya que el Input del motor ya maneja los eventos
    // Se puede expandir más adelante para integrar mejor con RmlUi
}

bool RmlUiInterface::LoadDocument(const std::string& document_path)
{
    if (!initialized || !context)
    {
        std::cerr << "RmlUiInterface: No inicializado" << std::endl;
        return false;
    }

    // Convertir a path absoluto si es relativo
    fs::path path = document_path;
    if (path.is_relative())
    {
        // Asumir que los documentos RML están en una carpeta "ui" en el directorio del proyecto
        fs::path project_root = fs::path(__FILE__).parent_path().parent_path();
        path = project_root / "ui" / document_path;
    }

    if (!fs::exists(path))
    {
        std::cerr << "RmlUiInterface: Documento no encontrado: " << path << std::endl;
        return false;
    }

    Rml::ElementDocument* document = context->LoadDocument(path.string());
    if (!document)
    {
        std::cerr << "RmlUiInterface: Error al cargar documento: " << path << std::endl;
        return false;
    }

    document->Show();
    main_menu_document = document;
    main_menu_visible = true;
    
    // Buscar el botón "START GAME" y asignar el listener
    Rml::Element* start_button = document->GetElementById("start-button");
    
    std::cout << "RmlUiInterface: Buscando botón START GAME..." << std::endl;
    
    if (!start_button)
    {
        std::cout << "RmlUiInterface: No se encontró por ID, buscando por texto..." << std::endl;
        // Si no tiene id, buscar por texto (menos confiable pero funciona)
        Rml::ElementList buttons;
        document->GetElementsByTagName(buttons, "button");
        std::cout << "RmlUiInterface: Encontrados " << buttons.size() << " botones" << std::endl;
        
        for (Rml::Element* button : buttons)
        {
            Rml::String text = button->GetInnerRML();
            std::cout << "RmlUiInterface: Botón encontrado con texto: '" << text.c_str() << "'" << std::endl;
            if (text.find("START GAME") != Rml::String::npos)
            {
                start_button = button;
                std::cout << "RmlUiInterface: Botón START GAME encontrado por texto" << std::endl;
                break;
            }
        }
    }
    else
    {
        std::cout << "RmlUiInterface: Botón START GAME encontrado por ID" << std::endl;
    }
    
    if (start_button)
    {
        std::cout << "RmlUiInterface: Botón encontrado, ID: '" 
                  << (start_button->GetId().empty() ? "sin ID" : start_button->GetId().c_str()) 
                  << "', Tag: '" << start_button->GetTagName().c_str() << "'" << std::endl;
        
        if (start_button_listener)
        {
            // Agregar el listener
            start_button->AddEventListener(Rml::EventId::Click, start_button_listener.get());
            start_button->AddEventListener(Rml::EventId::Mousedown, start_button_listener.get());
            
            std::cout << "RmlUiInterface: Listener asignado al botón START GAME (Click y Mousedown)" << std::endl;
            
            // Verificar que el elemento está visible y puede recibir eventos
            std::cout << "RmlUiInterface: Botón visible: " << (start_button->IsVisible() ? "Sí" : "No") << std::endl;
            std::cout << "RmlUiInterface: Botón posición: (" << start_button->GetAbsoluteLeft() 
                      << ", " << start_button->GetAbsoluteTop() << ")" << std::endl;
            std::cout << "RmlUiInterface: Botón tamaño: " << start_button->GetClientWidth() 
                      << "x" << start_button->GetClientHeight() << std::endl;
        }
        else
        {
            std::cerr << "RmlUiInterface: ERROR: start_button_listener es nullptr" << std::endl;
        }
    }
    else
    {
        std::cerr << "RmlUiInterface: ERROR: No se encontró el botón START GAME" << std::endl;
    }
    
    std::cout << "RmlUiInterface: Documento cargado: " << path << std::endl;
    return true;
}

void RmlUiInterface::ShowMainMenu()
{
    if (main_menu_document && !main_menu_visible)
    {
        main_menu_document->Show();
        main_menu_visible = true;
        std::cout << "RmlUiInterface: Menú principal mostrado" << std::endl;
    }
}

void RmlUiInterface::HideMainMenu()
{
    if (main_menu_document && main_menu_visible)
    {
        main_menu_document->Hide();
        main_menu_visible = false;
        std::cout << "RmlUiInterface: Menú principal ocultado" << std::endl;
        
        // Ejecutar callback si está definido
        if (on_menu_hidden_callback)
        {
            on_menu_hidden_callback();
        }
    }
}

void RmlUiInterface::SetOnMenuHiddenCallback(std::function<void()> callback)
{
    on_menu_hidden_callback = callback;
}

void RmlUiInterface::SetOnRestartCallback(std::function<void()> callback)
{
    on_restart_callback = callback;
}

bool RmlUiInterface::LoadPauseMenu()
{
    if (!initialized || !context)
    {
        std::cerr << "RmlUiInterface: No inicializado para cargar menú de pausa" << std::endl;
        return false;
    }

    // Convertir a path absoluto si es relativo
    fs::path path = "pause_menu.rml";
    if (path.is_relative())
    {
        fs::path project_root = fs::path(__FILE__).parent_path().parent_path();
        path = project_root / "ui" / "pause_menu.rml";
    }

    if (!fs::exists(path))
    {
        std::cerr << "RmlUiInterface: Menú de pausa no encontrado: " << path << std::endl;
        return false;
    }

    Rml::ElementDocument* document = context->LoadDocument(path.string());
    if (!document)
    {
        std::cerr << "RmlUiInterface: Error al cargar menú de pausa: " << path << std::endl;
        return false;
    }

    // No mostrar el documento todavía, solo cargarlo
    pause_menu_document = document;
    
    // Buscar y asignar listeners a los botones
    Rml::Element* continue_button = document->GetElementById("continue-button");
    Rml::Element* restart_button = document->GetElementById("restart-button");
    
    if (continue_button && continue_button_listener)
    {
        continue_button->AddEventListener(Rml::EventId::Click, continue_button_listener.get());
        std::cout << "RmlUiInterface: Listener asignado al botón CONTINUAR" << std::endl;
    }
    
    if (restart_button && restart_button_listener)
    {
        restart_button->AddEventListener(Rml::EventId::Click, restart_button_listener.get());
        std::cout << "RmlUiInterface: Listener asignado al botón REINICIAR" << std::endl;
    }
    
    std::cout << "RmlUiInterface: Menú de pausa cargado: " << path << std::endl;
    return true;
}

void RmlUiInterface::ShowPauseMenu()
{
    // Cargar el menú de pausa si no está cargado
    if (!pause_menu_document)
    {
        if (!LoadPauseMenu())
        {
            std::cerr << "RmlUiInterface: No se pudo cargar el menú de pausa" << std::endl;
            return;
        }
    }
    
    if (pause_menu_document && !pause_menu_visible)
    {
        pause_menu_document->Show();
        pause_menu_visible = true;
        std::cout << "RmlUiInterface: Menú de pausa mostrado" << std::endl;
    }
}

void RmlUiInterface::HidePauseMenu()
{
    if (pause_menu_document && pause_menu_visible)
    {
        pause_menu_document->Hide();
        pause_menu_visible = false;
        std::cout << "RmlUiInterface: Menú de pausa ocultado" << std::endl;
    }
}

