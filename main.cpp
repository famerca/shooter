#include <iostream>
#include <string>
#include <stdexcept>
#include <random>

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
    bool is_jumping{false};  
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
            object->getBody()->ApplyImpulse({0.f, 1500.f, 0.f});
            is_jumping = true;
        }


        if(is_key_pressed(GLFW_KEY_LEFT_SHIFT))
        {
            object->getBody()->ApplyImpulse({0.f, -10.f, 0.f});
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

int main()
{
    try
    {
        Engine::Physics::Get().Init();

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

        scene->at(user)->getTransform()->translate(glm::vec3(0.f, 0.f, 0.f));
        scene->at(user)->getTransform()->rotate(-90.f, glm::vec3(1.f, 0.f, 0.f));
        scene->at(user)->setBody(Engine::Physics::Get().CreateBox({0.3f, 0.3f, 0.3f}, {0.f, 0.f, 0.f}, true));
        
        camera->setFront(-20.f, 90.f);
        camera->setUp(glm::vec3(0.f, 1.f, 0.f));

        

        camera->init(glm::vec3(0.f, 1.5f, -2.5f), main_window->get_aspect_ratio(), 45.f, 0.1f, 100.f, true);
        camera->activate();
        
        //scene->at(user)->getTransform()->rotate(-20.f, glm::vec3(1.f, 0.f, 0.f));
       
        scene->createModel(user)->loadModel("pj/base.fbx");
        auto sphere = scene->createGameObject();
        scene->createModel(sphere)->loadModel("ground/base.fbx");

        scene->at(sphere)->getTransform()->translate(1.f, 0.f, -1.f);

        auto dado = scene->createGameObject();
        scene->createModel(dado)->loadModel("cuteplant/base.fbx");
        scene->at(dado)->getTransform()->translate(-1.f, 0.f, -0.f);


        std::cout << "cargando cofre\n";

        auto cilindro = scene->createGameObject();
        scene->createModel(cilindro)->loadModel("cofre/base.fbx");
        scene->at(cilindro)->getTransform()->translate(0.f, 0.f, 1.5f);
        scene->at(cilindro)->getTransform()->rotate(-90.f, glm::vec3(0.f, 0.f, 0.f));
        
        std::cout << "cargando casito\n";
        auto dado2 = scene->createGameObject();
        //scene->createModel(dado2)->loadModel("dado.fbx");
        //scene->createModel(dado2)->loadModel("filthy-prison-toilet-4096px2/source/toilet.fbx");
        scene->createModel(dado2)->loadModel("casita/base.fbx");
        scene->at(dado2)->getTransform()->translate(1.f, 0.5f, 3.f);
        scene->at(dado2)->getTransform()->rotate(-90.f, glm::vec3(1.f, 0.f, 0.f));

        std::cout << "cargando suelo\n";
        auto ground = scene->createGameObject();
        scene->createModel(ground)->loadModel("ground/base.fbx");
        scene->at(ground)->getTransform()->translate(0.f, -0.5f, 0.f);
        scene->at(ground)->getTransform()->scale(2.f, 2.f, 1.f);
        scene->at(ground)->getTransform()->rotate(-90.f, glm::vec3(1.f, 0.f, 0.f));

        std::cout << "final suelo" << std::endl;

        std::cout << "setting body suelo" << std::endl;
        scene->at(ground)->setBody(Engine::Physics::Get().CreateBox({2.f, 2.f, 0.5f}, {0.f, 0.f, 0.f}, false));

       
        std::cout << "definiendo obstaculos suelo" << std::endl;
        auto obstacles = std::make_shared<std::vector<unsigned>>();
         std::cout << "definiendo skybox" << std::endl;


        std::shared_ptr<SkyBox> sky_box = std::make_shared<SkyBox>(
            fs::path{__FILE__}.parent_path(),
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
        scene->at(sphere)->setBody(Engine::Physics::Get().CreateBox({0.4f, 0.4f, 0.4f}, {0.f, 0.f, 0.f}, true));

        //scene->at(sphere)->setBody(Engine::Physics::Get().CreateSphere(0.4f, {0.f, 0.f, 0.f}, true));
        scene->at(sphere)->getBody()->serVelocity({10.f, 0.f, 0.f});

        scene->at(dado)->getTransform()->scale(0.4f, 0.4f, 0.4f);
        scene->at(dado)->setBody(Engine::Physics::Get().CreateBox({0.5f, 0.5f, 0.5f}, {0.f, 0.f, 0.f}, true));

        scene->at(user)->getTransform()->scale(0.4f, 0.4f, 0.4f);

        scene->at(cilindro)->getTransform()->scale(0.4f, 0.4f, 0.8f);
        scene->at(cilindro)->setBody(Engine::Physics::Get().CreateBox({0.4f, 0.4f, 0.8f}, {0.f, 0.f, 0.f}, true));

        scene->at(dado2)->getTransform()->scale(0.8f, 0.8f, 0.4f);
        scene->at(dado2)->setBody(Engine::Physics::Get().CreateBox({0.8f, 0.8f, 0.4f}, {0.f, 0.f, 0.f}, true));

        

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
        std::cout << "init debug" << std::endl;
        renderer->debug();
        renderer->init();
        std::cout << "render scene" << std::endl;
        renderer->render(scene);

        Engine::Physics::Get().Shutdown();

    }catch(const std::exception& e)
    {

    }
    
    return EXIT_SUCCESS;
}
