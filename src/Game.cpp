#include "Game.hpp"
#include "inputManager.hpp"
#include "Level.hpp"
#include <GLS/DirectionalLight.hpp>
#include <GLS/Physics.hpp>
#include <GLS/SkyBox.hpp>
#include <GLS/Path.hpp>
#include <GLS/UIManager.hpp>
#include <iostream>

Game::Game(std::shared_ptr<Engine::Window> window)
    : m_window(window)
{
    m_renderer = std::make_shared<Engine::Renderer>();
    m_input = std::make_shared<inputManager>();
}

Game::~Game()
{

}

void Game::init()
{
    initScene();
    initSkyBox();
    initUser();
    initRenderer();
    //initGround();
    initInput();
    initCollitions();
    initUI();

    handleGameOver();
}

void Game::initScene()
{
    auto light = std::make_shared<Engine::DirectionalLight>(1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.5f);
    m_scene = Engine::Scene::create(m_window, light);
    m_user_index = m_scene->createGameObject();
    m_user = m_scene->at(m_user_index);
}

void Game::initInput()
{
    m_input->init(m_scene, m_user);
    m_window->setInput(m_input);
}

void Game::initUser()
{
    m_camera = m_scene->createCamera(m_user_index);
    m_camera->setClamp(70.0f, 70.0f);
    m_camera->setOrbiting(true);
    m_camera->setFront(-20.f, 0.f);
    m_camera->setUp(glm::vec3(0.f, 1.f, 0.f));

    m_camera->init(glm::vec3(0.f, 1.5f, -2.5f), m_window->get_aspect_ratio(), 45.f, 0.1f, 100.f, true);
    
    m_camera->activate();
    m_user->getTransform()->translate(-2.f, 0.f, 0.f);
    m_user->getTransform()->scale(0.8f, 0.8f, 0.8f);
    m_scene->createAudioListener(m_user_index);



    auto pj_model = m_scene->createModel(m_user_index);
    pj_model->loadModel("girl.fbx");
    pj_model->setRelativeModel(glm::vec3(0.f, -0.72f, 0.f));

    m_user->setBody(Engine::Physics::Get().CreateBox({0.25f, 0.6f, 0.25f}, {0.f, 0.0f, 0.f}, Engine::BodyType::Dynamic));
}

void Game::initGround()
{
    auto ground = m_scene->createGameObject();
    m_scene->createModel(ground)->loadModel("ground/base.fbx");
    m_scene->at(ground)->getTransform()->translate(0.f, -0.5f, 0.f);
    m_scene->at(ground)->getTransform()->scale(5.f, 1.f, 5.f);
    m_scene->at(ground)->setBody(Engine::Physics::Get().CreateBox({5.f, 0.5f, 5.f}, {0.f, 0.f, 0.f}, Engine::BodyType::Static));
}

void Game::initRenderer()
{
    m_renderer->debug();
    m_renderer->init();
}


void Game::initSkyBox()
{
    auto sky_box = std::make_shared<Engine::SkyBox>(
        "skybox",
        std::vector<Engine::fs::path>{{
            "cupertin-lake_rt.tga",
            "cupertin-lake_lf.tga",
            "cupertin-lake_up.tga",
            "cupertin-lake_dn.tga",
            "cupertin-lake_bk.tga",
            "cupertin-lake_ft.tga",
        }}
    );

    m_scene->setSkyBox(sky_box);
}


void Game::Level1()
{
    Level level1(m_scene, m_user_index);

    ObstacleSettings s_ground;;

    s_ground.box_shape = {5.f, 0.5f, 5.f};
    s_ground.body_type = Engine::BodyType::Static;
    s_ground.onContactStart = ground_collition;
    s_ground.user_index = m_user_index;
    s_ground.scale = {5.f, 1.f, 5.f};


    ObstacleSettings s_casita = {};

    s_casita.scale = {3.f, 3.f, 3.f};
    s_casita.box_shape = {2.f, 1.5f, 2.f};
    s_casita.rel_pos = {0.f, -0.75f, 0.f};
    s_casita.body_type = Engine::BodyType::Static;

    ObstacleSettings s_plataforma = {};

    s_plataforma.scale = {1.f, 1.f, 1.f};
    s_plataforma.box_shape = {1.f, 0.25f, 1.f};
    s_plataforma.body_type = Engine::BodyType::Static;
    s_plataforma.onContactStart = ground_collition;
    s_plataforma.user_index = m_user_index;
    s_plataforma.rel_pos = {0.f, -0.25f, 0.f};


    level1.init({
        {"casita/base.fbx", "deco", {2.f, 1.8f, 0.f}, s_casita}, 
        {"ground/base.fbx", "ground", {0.f, -0.5f, 0.f}, s_ground},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 7.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 10.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 14.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 16.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 20.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 24.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 30.f}, s_plataforma},
    });

}

void Game::render()
{
    m_renderer->render(m_scene);
}

void Game::initCollitions()
{
    ground_collition = [this]() {
        std::cout << "Inicio de colisión"  << std::endl;
        // Aquí puedes ejecutar lógica como aplicar daño o activar efectos
        m_input->setJumping(false);
    };

}


void Game::handleGameOver() noexcept
{
    m_input->setOnGameOver([this]() {
        std::cout << "=============GAME OVER=============" << std::endl;
        
        // Ocultar el contador de vidas
        m_ui_manager->HideTemplate("lives_counter");
        
        
        // Mostrar el menú de Game Over
        m_ui_manager->ShowTemplate("gameover");
        
        m_input->setOnGameOver(nullptr);
    });
}

void Game::initUI()
{
    // Inicializar UIManager
    m_ui_manager = std::make_shared<UIManager>();
    if (m_ui_manager->Initialize(m_window, "ui"))
    {
        // Cargar plantillas
        m_ui_manager->LoadTemplate("main_menu", "test.rml", true); // Mostrar automáticamente
        m_ui_manager->LoadTemplate("pause_menu", "pause_menu.rml", false); // Cargar pero no mostrar
        m_ui_manager->LoadTemplate("lives_counter", "lives_counter.rml", false); // Cargar pero no mostrar
        m_ui_manager->LoadTemplate("gameover", "gameover.rml", false); // Cargar pero no mostrar
        
        // Registrar eventos con lambdas
        // Evento del botón START GAME
        m_ui_manager->RegisterEvent("main_menu", "start-button", Rml::EventId::Click, 
            [this](Rml::Element*, Rml::EventId) {
                std::cout << "Botón START GAME presionado" << std::endl;
                m_ui_manager->HideTemplate("main_menu");
                m_ui_manager->ShowTemplate("lives_counter");
                std::cout << "Juego iniciado desde el menú" << std::endl;
            });
        
        // Evento del botón CONTINUAR
        m_ui_manager->RegisterEvent("pause_menu", "continue-button", Rml::EventId::Click,
            [this](Rml::Element*, Rml::EventId) {
                std::cout << "Botón CONTINUAR presionado" << std::endl;
                m_ui_manager->HideTemplate("pause_menu");
            });
        
        // Evento del botón REINICIAR JUEGO
        m_ui_manager->RegisterEvent("pause_menu", "restart-button", Rml::EventId::Click,
            [this](Rml::Element*, Rml::EventId) {
                std::cout << "Botón REINICIAR JUEGO presionado" << std::endl;
                m_ui_manager->HideTemplate("pause_menu");
                resetGame();
            });
        
        // Eventos del menú Game Over
        // Botón REINICIAR
        m_ui_manager->RegisterEvent("gameover", "restart-button", Rml::EventId::Click,
            [this](Rml::Element*, Rml::EventId) {
                std::cout << "Botón REINICIAR presionado (Game Over)" << std::endl;
                m_ui_manager->HideTemplate("gameover");
                resetGame();
            });
        
        // Botón MENÚ PRINCIPAL
        m_ui_manager->RegisterEvent("gameover", "main-menu-button", Rml::EventId::Click,
            [this](Rml::Element*, Rml::EventId) {
                std::cout << "Botón MAIN MENU presionado (Game Over)" << std::endl;
                m_ui_manager->HideTemplate("gameover");
                m_ui_manager->HideTemplate("lives_counter");
                resetGame();
                m_ui_manager->ShowTemplate("main_menu");
            });
        
        // Botón SALIR
        m_ui_manager->RegisterEvent("gameover", "exit-button", Rml::EventId::Click,
            [this](Rml::Element*, Rml::EventId) {
                std::cout << "Botón SALIR presionado (Game Over)" << std::endl;
                m_renderer->stop();
            });
        
        // Inicializar contador de vidas
        m_ui_manager->UpdateElementText("lives_counter", "lives-count", "3");
        
        // Conectar UIManager al renderer
        m_renderer->setUIManager(m_ui_manager);
        
        // Pasar ui_manager al inputManager para que pueda manejar la pausa con ESC
        m_input->init(m_scene, m_user, m_ui_manager);
        
        std::cout << "UIManager integrado correctamente. El juego está pausado hasta presionar START GAME." << std::endl;
    }
    else
    {
        std::cerr << "Error al inicializar UIManager, continuando sin interfaz" << std::endl;
    }
}

void Game::resetGame() noexcept
{
    std::cout << "=============REINICIAR JUEGO=============" << std::endl;
    std::cout << "Llamando a función de reset del Engine..." << std::endl;
    // Aquí se llamará a la función que tu amigo está implementando
    // Por ahora solo imprimimos el mensaje
}

void Game::shutdownUI() noexcept
{
    // Hacer shutdown explícito de UIManager antes de que se destruya Game
    // Esto asegura que el shutdown ocurra mientras el contexto OpenGL todavía existe
    if (m_ui_manager && m_ui_manager->IsInitialized())
    {
        // Desconectar UIManager del renderer primero
        if (m_renderer)
        {
            m_renderer->setUIManager(nullptr);
        }
        
        try
        {
            std::cout << "Shutting down UIManager..." << std::endl;
            
            // La librería GLS ya tiene las correcciones en UIManager::Shutdown()
            // que desregistran eventos antes de cerrar documentos, así que simplemente
            // llamamos a Shutdown() y la librería se encarga del resto
            m_ui_manager->Shutdown();
            std::cout << "UIManager shutdown completado" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error durante UIManager shutdown: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Error desconocido durante UIManager shutdown" << std::endl;
        }
    }
}