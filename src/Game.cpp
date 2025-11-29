#include "Game.hpp"
#include "inputManager.hpp"
#include "Level.hpp"
#include <GLS/DirectionalLight.hpp>
#include <GLS/Physics.hpp>
#include <GLS/SkyBox.hpp>
#include <GLS/Path.hpp>

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
    s_plataforma.body_type = Engine::BodyType::Kinematic;
    s_plataforma.onContactStart = ground_collition;
    s_plataforma.user_index = m_user_index;
    s_plataforma.rel_pos = {0.f, -0.25f, 0.f};

    ObstacleSettings s_plataforma2 = s_plataforma;

    s_plataforma2.script = "PlataformaMovil";


    level1.init({
        {"casita/base.fbx", "deco", {2.f, 1.8f, 0.f}, s_casita}, 
        {"ground/base.fbx", "ground", {0.f, -0.5f, 0.f}, s_ground},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 7.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 10.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 14.f}, s_plataforma},
        {"ground/base.fbx", "plataforma", {-2.f, -0.5f, 16.f}, s_plataforma2},
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
        m_renderer->stop();
       // exit(0);

    });
}