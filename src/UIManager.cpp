#include "UIManager.hpp"
#include "Window.hpp"
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Element.h>
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

UIManager::UIManager()
    : context(nullptr)
    , render_interface(nullptr)
    , system_interface(nullptr)
    , initialized(false)
    , window_width(0)
    , window_height(0)
    , ui_directory("ui")
{
}

UIManager::~UIManager()
{
    Shutdown();
}

bool UIManager::Initialize(std::shared_ptr<Engine::Window> window, const std::string& ui_directory)
{
    if (initialized)
    {
        std::cerr << "UIManager: Ya está inicializado" << std::endl;
        return false;
    }

    if (!window || !window->getGLFWWindow())
    {
        std::cerr << "UIManager: Ventana inválida" << std::endl;
        return false;
    }

    this->window = window;
    this->ui_directory = ui_directory;
    GLFWwindow* glfw_window = window->getGLFWWindow();

    // Obtener dimensiones de la ventana
    glfwGetFramebufferSize(glfw_window, &window_width, &window_height);

    // Inicializar RmlGL3
    Rml::String renderer_message;
    if (!RmlGL3::Initialize(&renderer_message))
    {
        std::cerr << "UIManager: Error al inicializar RmlGL3" << std::endl;
        return false;
    }
    std::cout << "UIManager: " << renderer_message << std::endl;

    // Crear las interfaces de renderizado y sistema
    render_interface = std::make_unique<RenderInterface_GL3>();
    if (!*render_interface)
    {
        std::cerr << "UIManager: Error al crear RenderInterface" << std::endl;
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
        std::cerr << "UIManager: Error al inicializar RmlUi" << std::endl;
        render_interface.reset();
        system_interface.reset();
        RmlGL3::Shutdown();
        return false;
    }

    // Crear el contexto principal
    context = Rml::CreateContext("main", Rml::Vector2i(window_width, window_height));
    if (!context)
    {
        std::cerr << "UIManager: Error al crear contexto" << std::endl;
        Rml::Shutdown();
        render_interface.reset();
        system_interface.reset();
        RmlGL3::Shutdown();
        return false;
    }

    // Inicializar el debugger (opcional, presiona F8 para activarlo)
    Rml::Debugger::Initialise(context);

    // Cargar fuentes
    fs::path project_root = fs::path(__FILE__).parent_path().parent_path();
    fs::path font_paths[] = {
        project_root / ui_directory / "fonts" / "LatoLatin-Regular.ttf",
        project_root / "build" / "third_party" / "RmlUi" / "Samples" / "assets" / "LatoLatin-Regular.ttf"
    };

    bool font_loaded = false;
    for (const auto& font_path : font_paths)
    {
        if (fs::exists(font_path))
        {
            if (Rml::LoadFontFace(font_path.string()))
            {
                std::cout << "UIManager: Fuente cargada: " << font_path << std::endl;
                font_loaded = true;
                break;
            }
        }
    }

    if (!font_loaded)
    {
        std::cerr << "UIManager: Advertencia: No se pudo cargar ninguna fuente. Los documentos pueden no renderizarse correctamente." << std::endl;
    }

    initialized = true;
    std::cout << "UIManager: Inicializado correctamente" << std::endl;

    return true;
}

void UIManager::Shutdown()
{
    if (!initialized)
        return;

    // Limpiar todas las plantillas
    templates.clear();
    event_registrations.clear();

    if (context)
    {
        Rml::Debugger::Shutdown();
        Rml::RemoveContext("main");
        context = nullptr;
    }

    Rml::Shutdown();
    render_interface.reset();
    system_interface.reset();
    RmlGL3::Shutdown();

    initialized = false;
    std::cout << "UIManager: Cerrado correctamente" << std::endl;
}

void UIManager::Update()
{
    if (!initialized || !context)
        return;

    context->Update();
}

void UIManager::Render()
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

std::string UIManager::GetFullPath(const std::string& file_path) const
{
    fs::path path = file_path;
    if (path.is_absolute())
    {
        return path.string();
    }
    
    // Si es relativo, asumir que está en ui_directory
    fs::path project_root = fs::path(__FILE__).parent_path().parent_path();
    return (project_root / ui_directory / file_path).string();
}

bool UIManager::LoadTemplate(const std::string& template_id, const std::string& file_path, bool auto_show)
{
    if (!initialized || !context)
    {
        std::cerr << "UIManager: No inicializado" << std::endl;
        return false;
    }

    // Verificar si ya existe una plantilla con este ID
    if (templates.find(template_id) != templates.end())
    {
        std::cerr << "UIManager: Ya existe una plantilla con ID: " << template_id << std::endl;
        return false;
    }

    std::string full_path = GetFullPath(file_path);
    
    if (!fs::exists(full_path))
    {
        std::cerr << "UIManager: Archivo no encontrado: " << full_path << std::endl;
        return false;
    }

    Rml::ElementDocument* document = context->LoadDocument(full_path);
    if (!document)
    {
        std::cerr << "UIManager: Error al cargar documento: " << full_path << std::endl;
        return false;
    }

    // Guardar información de la plantilla
    TemplateInfo info;
    info.template_id = template_id;
    info.document = document;
    info.visible = auto_show;
    info.file_path = full_path;
    
    if (auto_show)
    {
        document->Show();
    }
    else
    {
        document->Hide();
    }

    templates[template_id] = info;
    std::cout << "UIManager: Plantilla '" << template_id << "' cargada desde: " << full_path << std::endl;

    return true;
}

bool UIManager::UnloadTemplate(const std::string& template_id)
{
    auto it = templates.find(template_id);
    if (it == templates.end())
    {
        std::cerr << "UIManager: Plantilla no encontrada: " << template_id << std::endl;
        return false;
    }

    // Remover eventos asociados a esta plantilla
    event_registrations.erase(
        std::remove_if(event_registrations.begin(), event_registrations.end(),
            [&template_id](const EventRegistration& reg) {
                return reg.template_id == template_id;
            }),
        event_registrations.end()
    );

    // Cerrar y remover el documento
    if (it->second.document)
    {
        it->second.document->Close();
    }

    templates.erase(it);
    std::cout << "UIManager: Plantilla '" << template_id << "' descargada" << std::endl;

    return true;
}

bool UIManager::ShowTemplate(const std::string& template_id)
{
    auto it = templates.find(template_id);
    if (it == templates.end())
    {
        std::cerr << "UIManager: Plantilla no encontrada: " << template_id << std::endl;
        return false;
    }

    if (it->second.document && !it->second.visible)
    {
        it->second.document->Show();
        it->second.visible = true;
        std::cout << "UIManager: Plantilla '" << template_id << "' mostrada" << std::endl;
    }

    return true;
}

bool UIManager::HideTemplate(const std::string& template_id)
{
    auto it = templates.find(template_id);
    if (it == templates.end())
    {
        std::cerr << "UIManager: Plantilla no encontrada: " << template_id << std::endl;
        return false;
    }

    if (it->second.document && it->second.visible)
    {
        it->second.document->Hide();
        it->second.visible = false;
        std::cout << "UIManager: Plantilla '" << template_id << "' ocultada" << std::endl;
    }

    return true;
}

bool UIManager::IsTemplateVisible(const std::string& template_id) const
{
    auto it = templates.find(template_id);
    if (it == templates.end())
    {
        return false;
    }
    return it->second.visible;
}

bool UIManager::IsTemplateLoaded(const std::string& template_id) const
{
    return templates.find(template_id) != templates.end();
}

bool UIManager::RegisterEvent(const std::string& template_id, 
                             const std::string& element_id,
                             Rml::EventId event_type,
                             EventCallback callback)
{
    auto it = templates.find(template_id);
    if (it == templates.end())
    {
        std::cerr << "UIManager: Plantilla no encontrada: " << template_id << std::endl;
        return false;
    }

    if (!it->second.document)
    {
        std::cerr << "UIManager: Documento de plantilla inválido: " << template_id << std::endl;
        return false;
    }

    Rml::Element* element = it->second.document->GetElementById(element_id);
    if (!element)
    {
        std::cerr << "UIManager: Elemento no encontrado: " << element_id << " en plantilla: " << template_id << std::endl;
        return false;
    }

    // Crear listener genérico
    auto listener = std::make_unique<GenericEventListener>(callback);
    
    // Registrar el evento
    element->AddEventListener(event_type, listener.get());

    // Guardar el registro
    EventRegistration reg;
    reg.template_id = template_id;
    reg.element_id = element_id;
    reg.event_type = event_type;
    reg.callback = callback;
    reg.listener = std::move(listener);
    
    event_registrations.push_back(std::move(reg));
    
    std::cout << "UIManager: Evento registrado para elemento '" << element_id 
              << "' en plantilla '" << template_id << "'" << std::endl;

    return true;
}

bool UIManager::UpdateElementText(const std::string& template_id, const std::string& element_id, const std::string& text)
{
    auto it = templates.find(template_id);
    if (it == templates.end())
    {
        std::cerr << "UIManager: Plantilla no encontrada: " << template_id << std::endl;
        return false;
    }

    if (!it->second.document)
    {
        std::cerr << "UIManager: Documento de plantilla inválido: " << template_id << std::endl;
        return false;
    }

    Rml::Element* element = it->second.document->GetElementById(element_id);
    if (!element)
    {
        std::cerr << "UIManager: Elemento no encontrado: " << element_id << " en plantilla: " << template_id << std::endl;
        return false;
    }

    element->SetInnerRML(text.c_str());
    return true;
}

Rml::Element* UIManager::GetElement(const std::string& template_id, const std::string& element_id) const
{
    auto it = templates.find(template_id);
    if (it == templates.end() || !it->second.document)
    {
        return nullptr;
    }

    return it->second.document->GetElementById(element_id);
}

Rml::ElementDocument* UIManager::GetTemplateDocument(const std::string& template_id) const
{
    auto it = templates.find(template_id);
    if (it == templates.end())
    {
        return nullptr;
    }

    return it->second.document;
}

