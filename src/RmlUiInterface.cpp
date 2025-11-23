#include "RmlUiInterface.hpp"
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/Event.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// Clase listener para el botón de empezar
class RmlUiInterface::StartButtonListener : public Rml::EventListener {
public:
    StartButtonListener(RmlUiInterface* interface) : rmlui_interface(interface) {}
    
    void ProcessEvent(Rml::Event& event) override {
        if (event.GetId() == Rml::EventId::Click) {
            if (rmlui_interface) {
                rmlui_interface->HideMainMenu();
                std::cout << "RmlUiInterface: Botón empezar presionado" << std::endl;
            }
        }
    }
    
private:
    RmlUiInterface* rmlui_interface;
};

RmlUiInterface::RmlUiInterface()
    : context(nullptr)
    , render_interface(nullptr)
    , system_interface(nullptr)
    , initialized(false)
    , window_width(0)
    , window_height(0)
    , main_menu_visible(false)
    , main_menu_document(nullptr)
    , start_button_listener(nullptr)
{
}

RmlUiInterface::~RmlUiInterface()
{
    Shutdown();
}

bool RmlUiInterface::Initialize(std::shared_ptr<Window> window)
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
        if (main_menu_document)
        {
            Rml::Element* start_button = main_menu_document->GetElementById("start-button");
            if (start_button && start_button_listener)
            {
                start_button->RemoveEventListener(Rml::EventId::Click, start_button_listener.get());
            }
        }
        main_menu_document = nullptr;
        start_button_listener.reset();
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

void RmlUiInterface::ProcessInput(GLFWwindow* glfw_window)
{
    if (!initialized || !context || !glfw_window)
        return;

    // Procesar eventos de mouse para RmlUi
    // Esto se hace automáticamente si usamos los callbacks de GLFW
    // Por ahora, procesamos manualmente desde el Input del motor
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
    std::cout << "RmlUiInterface: Documento cargado: " << path << std::endl;
    
    // Si es el menú principal, guardar referencia y configurar eventos
    if (document_path.find("test.rml") != std::string::npos)
    {
        main_menu_document = document;
        main_menu_visible = true;
        SetupStartButtonListener();
    }
    
    return true;
}

void RmlUiInterface::ShowMainMenu()
{
    if (!initialized || !context)
        return;
    
    if (!main_menu_document)
    {
        // Cargar el menú si no está cargado
        LoadDocument("test.rml");
    }
    else
    {
        main_menu_document->Show();
        main_menu_visible = true;
    }
}

void RmlUiInterface::HideMainMenu()
{
    if (!initialized || !main_menu_document)
        return;
    
    main_menu_document->Hide();
    main_menu_visible = false;
    
    // Notificar que el juego debe reanudarse
    // Esto se manejará desde el renderer
}

void RmlUiInterface::SetupStartButtonListener()
{
    if (!main_menu_document)
        return;
    
    // Crear y configurar el listener para el botón de empezar
    start_button_listener = std::make_unique<StartButtonListener>(this);
    Rml::Element* start_button = main_menu_document->GetElementById("start-button");
    if (start_button)
    {
        start_button->AddEventListener(Rml::EventId::Click, start_button_listener.get());
    }
}

