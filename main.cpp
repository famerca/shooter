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

#include <Window.hpp>
#include "Scene.hpp"
#include "GameObject.hpp"
#include "ModelComponent.hpp"
#include "TransformComponent.hpp"
#include "DirectionalLight.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include "SkyBox.hpp"
#include "Physics.hpp"
#include "Listener.hpp"
#include "AudioManager.hpp"
#include "AudioPlayer.hpp"
#include "AudioListenerComponent.hpp"
#include "AudioSourceComponent.hpp"
#include "Path.hpp"
#include "UIManager.hpp"

using namespace Engine;

// Definir inputManager antes de las funciones helper
class inputManager: public Input
{
    // Atributos del Jugador
    std::shared_ptr<GameObject> object{nullptr};
    std::shared_ptr<Scene> scene{nullptr};
    float sensitivity{1.f};
    float speed{3.f}; // Velocidad de movimiento horizontal

    // Atributos de Salto
    float jump_velocity{10.f}; 
    float gravity{-20.f};     
    float current_y_velocity{0.f}; 
     
    float initial_y{0.f};     

    // Atributos de Obstáculos
    // Lista de IDs de los obstáculos (Compartida desde main)
    std::shared_ptr<std::vector<unsigned>> obstacles_list{nullptr}; 
    const float OBSTACLE_SPEED = 3.0f; // Velocidad con la que avanzan los obstáculos
    const float Z_RESET = 5.0f;        // Posición Z a la que reaparecen
    const float Z_LIMIT = -6.0f;       // Límite Z para reiniciar
    
    // Generador de números aleatorios para la posición X del obstáculo
    std::mt19937 gen;
    std::uniform_real_distribution<> distrib_x;

public:
    bool is_jumping {false};

    inputManager() : Input() 
    {
        // Inicialización del generador aleatorio en el constructor
        std::random_device rd;
        gen = std::mt19937(rd());
        // Rango para X aleatoria entre -1.0 y 1.0
        distrib_x = std::uniform_real_distribution<>(-1.0, 1.0);
    }

    void init(std::shared_ptr<Scene> scene, 
              std::shared_ptr<GameObject> object, 
              std::shared_ptr<std::vector<unsigned>> obstacles)
    {
        this->object = object;
        this->scene = scene;
        this->obstacles_list = obstacles; // Guarda la lista de IDs de obstáculos
    }

    void update(const float &dt) noexcept {
        glm::vec3 pos = object->getTransform()->getPosition();

        // 1. --- Lógica del Salto del Jugador (Y) ---
        if (is_key_pressed(GLFW_KEY_SPACE) && !is_jumping)
        {
            object->getBody()->ApplyImpulse({0.f, 1000.f, 0.f});
            is_jumping = true;
        }


        if(is_key_pressed(GLFW_KEY_LEFT_SHIFT))
        {
            object->getBody()->ApplyImpulse({0.f, -10.f, 0.f});
        }

        if(is_key_pressed(GLFW_KEY_LEFT))
        {
            scene->getCamera()->rotate(-50.f, 0.f);
        }

        if(is_key_pressed(GLFW_KEY_RIGHT))
        {
            scene->getCamera()->rotate(50.f, 0.f);
        }

        if(is_key_pressed(GLFW_KEY_UP))
        {
            scene->getCamera()->rotate(0.f, 50.f);
        }

        if(is_key_pressed(GLFW_KEY_DOWN))
        {
            scene->getCamera()->rotate(0.f, -50.f);
        }

        
        // 2. --- Lógica de Movimiento Horizontal del Jugador (X/Z) ---
        // Nota: Solo se usa translate para el movimiento horizontal.
        
        if(is_key_pressed(GLFW_KEY_A))
        {
            // Mover en X positivo (asumiendo que 'A' es para ir a la derecha en el plano XZ)
            object->getBody()->ApplyImpulse({30.f, 0.f, 0.f});
        }

        if(is_key_pressed(GLFW_KEY_D))
        {
            // Mover en X negativo (asumiendo que 'D' es para ir a la izquierda)
            object->getBody()->ApplyImpulse({-30.f, 0.f, 0.f});
        }
        
        if(is_key_pressed(GLFW_KEY_W))
        {
            object->getBody()->ApplyImpulse({0.f, 0.f, 40.f});
        }

        if(is_key_pressed(GLFW_KEY_S))
        {
            object->getBody()->ApplyImpulse({0.f, 0.f, -40.f});
        }
        
        // 3. --- Lógica de Movimiento y Reaparición de Obstáculos ---
        // if (obstacles_list && scene) 
        // {
        //     for (unsigned id : *obstacles_list) 
        //     {
        //         std::shared_ptr<GameObject> obstacle = scene->at(id);
        //         glm::vec3 obs_pos = obstacle->getTransform()->getPosition();

        //         // Mover el obstáculo hacia el usuario (Z negativa)
        //         obs_pos.z -= OBSTACLE_SPEED * dt;
                
        //         // Verificar si el obstáculo alcanzó el límite de reaparición
        //         if (obs_pos.z < Z_LIMIT) 
        //         {
        //             // Reiniciar la posición Z y asignar una X aleatoria (-1.0 a 1.0)
        //             obs_pos.z = Z_RESET;
        //             obs_pos.x = distrib_x(gen); 
        //         }

        //         // Aplicar la nueva posición
        //         obstacle->getTransform()->translate(obs_pos);
        //     }
        // }
        // -------------------------------------------------------------
        
        //std::cout << "delta: " << dt <<  ", FPS: " << 1.f / dt << std::endl;

    }

    ~inputManager() {}
};

// Estructura para almacenar el estado inicial del juego
struct GameInitialState {
    unsigned user_id;
    unsigned sphere_id;
    unsigned dado_id;
    unsigned cilindro_id;
    unsigned dado2_id;
    
    // Posiciones iniciales
    glm::vec3 user_initial_pos{0.f, 0.f, 0.f};
    glm::vec3 sphere_initial_pos{1.f, 0.f, -1.f};
    glm::vec3 dado_initial_pos{-1.f, 0.f, 0.f};
    glm::vec3 cilindro_initial_pos{0.f, 0.f, 1.5f};
    glm::vec3 dado2_initial_pos{1.f, 0.5f, 3.f};
    
    // Velocidades iniciales
    glm::vec3 sphere_initial_velocity{10.f, 0.f, 0.f};
};

// Función helper para resetear un GameObject a su posición inicial
void ResetGameObject(std::shared_ptr<Scene> scene, unsigned object_id, 
                     const glm::vec3& initial_pos, const glm::vec3& initial_velocity = glm::vec3(0.f))
{
    auto obj = scene->at(object_id);
    if (!obj) return;
    
    // Resetear transform (usando translate para establecer posición absoluta)
    auto current_pos = obj->getTransform()->getPosition();
    obj->getTransform()->translate(initial_pos - current_pos);
    
    // Resetear física si tiene body
    if (obj->getBody()) {
        obj->getBody()->SetPosition({initial_pos.x, initial_pos.y, initial_pos.z});
        obj->getBody()->SetVelocity({initial_velocity.x, initial_velocity.y, initial_velocity.z});
    }
}

// Función principal para reiniciar el juego
void ResetGameState(std::shared_ptr<Scene> scene, 
                    const GameInitialState& initial_state,
                    std::shared_ptr<inputManager> input)
{
    std::cout << "Reiniciando juego..." << std::endl;
    
    // Resetear usuario
    ResetGameObject(scene, initial_state.user_id, initial_state.user_initial_pos);
    
    // Resetear obstáculos
    ResetGameObject(scene, initial_state.sphere_id, 
                    initial_state.sphere_initial_pos, 
                    initial_state.sphere_initial_velocity);
    ResetGameObject(scene, initial_state.dado_id, initial_state.dado_initial_pos);
    ResetGameObject(scene, initial_state.cilindro_id, initial_state.cilindro_initial_pos);
    ResetGameObject(scene, initial_state.dado2_id, initial_state.dado2_initial_pos);
    
    // Resetear estado del input
    if (input) {
        input->is_jumping = false;
    }
    
    std::cout << "Juego reiniciado" << std::endl;
}

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

        // Inicializar UIManager (sistema genérico de UI)
        auto ui_manager = std::make_shared<UIManager>();
        if (ui_manager->Initialize(main_window, "ui"))
        {
            // Cargar plantillas
            ui_manager->LoadTemplate("main_menu", "test.rml", true); // Mostrar automáticamente
            ui_manager->LoadTemplate("pause_menu", "pause_menu.rml", false); // Cargar pero no mostrar
            ui_manager->LoadTemplate("lives_counter", "lives_counter.rml", false); // Cargar pero no mostrar
            
            // Crear estructura con el estado inicial del juego
            GameInitialState initial_state;
            initial_state.user_id = user;
            initial_state.sphere_id = sphere;
            initial_state.dado_id = dado;
            initial_state.cilindro_id = cilindro;
            initial_state.dado2_id = dado2;
            
            // Registrar eventos con lambdas (sistema genérico)
            // Evento del botón START GAME
            ui_manager->RegisterEvent("main_menu", "start-button", Rml::EventId::Click, 
                [ui_manager, renderer](Rml::Element*, Rml::EventId) {
                    std::cout << "Botón START GAME presionado" << std::endl;
                    ui_manager->HideTemplate("main_menu");
                    ui_manager->ShowTemplate("lives_counter");
                    std::cout << "Juego reanudado desde el menú" << std::endl;
                });
            
            // Evento del botón CONTINUAR
            ui_manager->RegisterEvent("pause_menu", "continue-button", Rml::EventId::Click,
                [ui_manager](Rml::Element*, Rml::EventId) {
                    std::cout << "Botón CONTINUAR presionado" << std::endl;
                    ui_manager->HideTemplate("pause_menu");
                });
            
            // Evento del botón REINICIAR JUEGO
            ui_manager->RegisterEvent("pause_menu", "restart-button", Rml::EventId::Click,
                [ui_manager, scene, initial_state, input](Rml::Element*, Rml::EventId) {
                    std::cout << "Botón REINICIAR JUEGO presionado" << std::endl;
                    ui_manager->HideTemplate("pause_menu");
                    ResetGameState(scene, initial_state, input);
                });
            
            // Inicializar contador de vidas
            ui_manager->UpdateElementText("lives_counter", "lives-count", "3");
            
            // Conectar UIManager al renderer
            renderer->setUIManager(ui_manager);
            std::cout << "UIManager integrado correctamente. El juego está pausado hasta presionar START GAME." << std::endl;
        }
        else
        {
            std::cerr << "Error al inicializar UIManager, continuando sin interfaz" << std::endl;
        }
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
