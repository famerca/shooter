#include <iostream>
#include <string>
#include <stdexcept>
#include <random>
#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLS/Window.hpp>
#include "GLS/Scene.hpp"
#include "GLS/GameObject.hpp"
#include "GLS/ModelComponent.hpp"
#include "GLS/TransformComponent.hpp"
#include "GLS/DirectionalLight.hpp"
#include "GLS/Renderer.hpp"
#include "GLS/Input.hpp"
#include "GLS/SkyBox.hpp"
#include "GLS/Physics.hpp"
#include "GLS/Listener.hpp"
#include "GLS/AudioManager.hpp"
#include "GLS/AudioPlayer.hpp"
#include "GLS/AudioListenerComponent.hpp"
#include "GLS/AudioSourceComponent.hpp"
#include "GLS/Path.hpp"

#include "inputManager.hpp"


using namespace Engine;

int main()
{
    try
    {
        
        Engine::Physics::Get().Init();
        Engine::AudioManager::Get().init();

        // Window dimensions
        constexpr GLint WIDTH = 1200;
        constexpr GLint HEIGHT = 720;


        auto main_window = Window::create(WIDTH, HEIGHT, "Cubo y piramide");

        if (main_window == nullptr)
        {
            return EXIT_FAILURE;
        }
        auto input = std::make_shared<inputManager>();
        main_window->setInput(input);

        auto light = std::make_shared<DirectionalLight>(1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.5f);
        auto scene = Scene::create(main_window, light);

        auto user = scene->createGameObject();
        auto camera = scene->createCamera(user);

        camera->setClamp(0.0f, 0.0f);
        camera->setOrbiting(false);

        scene->at(user)->getTransform()->translate(glm::vec3(0.f, 0.f, 0.f));
        
        camera->setFront(-20.f, 90.f);
        camera->setUp(glm::vec3(0.f, 1.f, 0.f));

        

        camera->init(glm::vec3(0.f, 1.5f, -2.5f), main_window->get_aspect_ratio(), 45.f, 0.1f, 100.f, true);
        camera->activate();

        scene->at(user)->addComponent(std::make_shared<Engine::AudioListenerComponent>(scene->at(user)));
        
        //scene->at(user)->getTransform()->rotate(-20.f, glm::vec3(1.f, 0.f, 0.f));
       
        auto pj_model = scene->createModel(user);
        pj_model->loadModel("pj/base.fbx");
        pj_model->setRelativeModel(glm::vec3(0.f, -1.f, 0.f));
    

        auto sphere = scene->createGameObject();
        scene->createModel(sphere)->loadModel("sphere.fbx");

        scene->at(sphere)->getTransform()->translate(1.f, 0.f, -1.f);

        auto dado = scene->createGameObject();
        scene->createModel(dado)->loadModel("dado.fbx");
        scene->at(dado)->getTransform()->translate(-1.f, 0.f, -0.f);

        auto cilindro = scene->createGameObject();
        scene->createModel(cilindro)->loadModel("cilinder.fbx");
        scene->at(cilindro)->getTransform()->translate(0.f, 0.f, 1.5f);
        scene->at(cilindro)->getTransform()->rotate(90.f, glm::vec3(0.f, 1.f, 0.f));

        auto dado2 = scene->createGameObject();
        //scene->createModel(dado2)->loadModel("dado.fbx");
        //scene->createModel(dado2)->loadModel("filthy-prison-toilet-4096px2/source/toilet.fbx");
        scene->createModel(dado2)->loadModel("casita/base.fbx");
        scene->at(dado2)->getTransform()->translate(1.f, 0.5f, 3.f);
        scene->at(dado2)->getTransform()->rotate(-90.f, glm::vec3(1.f, 0.f, 0.f));

        auto ground = scene->createGameObject();
        scene->createModel(ground)->loadModel("ground/base.fbx");
        scene->at(ground)->getTransform()->translate(0.f, -0.5f, 0.f);
        scene->at(ground)->getTransform()->scale(10.f, 1.f, 10.f);
        scene->at(ground)->setBody(Engine::Physics::Get().CreateBox({10.f, 0.5f, 10.f}, {0.f, 0.f, 0.f}, Engine::BodyType::Static));

       
       
        auto obstacles = std::make_shared<std::vector<unsigned>>();


        std::shared_ptr<SkyBox> sky_box = std::make_shared<SkyBox>(
            "skybox",
            std::vector<fs::path>{{
                "cupertin-lake_rt.tga",
                "cupertin-lake_lf.tga",
                "cupertin-lake_up.tga",
                "cupertin-lake_dn.tga",
                "cupertin-lake_bk.tga",
                "cupertin-lake_ft.tga",
            }}
        );

        scene->setSkyBox(sky_box);


        scene->at(sphere)->getTransform()->scale(0.4f, 0.4f, 0.4f);
        scene->at(sphere)->setBody(Engine::Physics::Get().CreateBox({0.4f, 0.4f, 0.4f}, {0.f, 0.f, 0.f}, Engine::BodyType::Kinematic));
        //scene->at(sphere)->setBody(Engine::Physics::Get().CreateSphere(0.4f, {0.f, 0.f, 0.f}, true));

        //scene->at(sphere)->setBody(Engine::Physics::Get().CreateSphere(0.4f, {0.f, 0.f, 0.f}, true));
        scene->at(sphere)->getBody()->SetVelocity({10.f, 0.f, 0.f});


        auto player = scene->createAudioSource(sphere, "Lost-Verdania.mp3", true, 1.0f, 1.0f, 20.0f);

        scene->at(dado)->getTransform()->scale(0.4f, 0.4f, 0.4f);
        scene->at(dado)->setBody(Engine::Physics::Get().CreateBox({0.5f, 0.5f, 0.5f}, {0.f, 0.f, 0.f}, Engine::BodyType::Dynamic));

        scene->at(user)->getTransform()->scale(0.4f, 0.4f, 0.4f);

        scene->at(cilindro)->getTransform()->scale(0.4f, 0.4f, 0.8f);
        scene->at(cilindro)->setBody(Engine::Physics::Get().CreateBox({0.4f, 0.4f, 0.8f}, {0.f, 0.f, 0.f}, Engine::BodyType::Dynamic));

        scene->at(dado2)->getTransform()->scale(0.8f, 0.8f, 0.4f);
        scene->at(dado2)->setBody(Engine::Physics::Get().CreateBox({0.8f, 0.8f, 0.4f}, {0.f, 0.f, 0.f}, Engine::BodyType::Dynamic));

        auto body = Engine::Physics::Get().CreateBox({0.2f, 0.4f, 0.2f}, {0.f, 0.f, 0.f}, Engine::BodyType::Dynamic);
        scene->at(user)->setBody(body);

        obstacles->push_back(sphere);
        obstacles->push_back(dado);
        obstacles->push_back(cilindro);
        obstacles->push_back(dado2);

        input->init(scene, scene->at(user), obstacles);


        // --- 1. Registrar la Lógica de INICIO DE COLISIÓN (OnContactAdded) ---
        auto onAdd = [input]() {
            std::cout << "Inicio de colisión"  << std::endl;
            // Aquí puedes ejecutar lógica como aplicar daño o activar efectos
            input->is_jumping = false;
        };

        auto onRemove = []() {
            std::cout << "Fin de colisión" << std::endl;
            // Aquí puedes ejecutar lógica como desactivar efectos o restaurar daño
        };

        Engine::Listener::Get().Add(scene, Engine::Listener::Event::ContactAdded, user, ground, onAdd);

        Engine::Listener::Get().Add(scene, Engine::Listener::Event::ContactRemoved, user, ground, onRemove);

        auto renderer = std::make_shared<Renderer>();
        scene->at(sphere)->getMovement()->moveTo(glm::vec3(0.f, 0.f, 10.f), 1.f);
        renderer->debug();
        renderer->init();
        std::cout << "Playing sound" << std::endl;
        std::filesystem::path audio_path = std::filesystem::path(__FILE__).parent_path() / "audios" / "Lost-Verdania.mp3";
        std::cout << "Audio path: " << audio_path.string() << std::endl;
        
        player->play();
        
        renderer->render(scene);

        Engine::Physics::Get().Shutdown();
        Engine::AudioManager::Get().shutdown();

    }catch(const std::exception& e)
    {

    }
    
    return EXIT_SUCCESS;
}
