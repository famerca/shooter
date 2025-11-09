#include <iostream>
#include <string>
#include <stdexcept>

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
#include <random>

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
    bool is_jumping{false};   
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
            is_jumping = true;
            current_y_velocity = jump_velocity; 
            initial_y = pos.y; 
        }

        if (is_jumping)
        {
            // Aplicar gravedad: v = v + a * dt
            current_y_velocity += gravity * dt; 

            // Calcular desplazamiento: y = y + v * dt
            float y_displacement = current_y_velocity * dt;
            
            // Mover el objeto
            pos.y += y_displacement;
            
            // Lógica de Aterrizaje
            if (pos.y <= initial_y)
            {
                pos.y = initial_y; 
                is_jumping = false;
                current_y_velocity = 0.f;
            }
            
            object->getTransform()->translate(pos); // Aplicamos la nueva posición Y
        }
        
        // 2. --- Lógica de Movimiento Horizontal del Jugador (X/Z) ---
        // Nota: Solo se usa translate para el movimiento horizontal.
        
        if(is_key_pressed(GLFW_KEY_A))
        {
            // Mover en X positivo (asumiendo que 'A' es para ir a la derecha en el plano XZ)
            if(pos.x < 1.5f)
                object->getTransform()->translate(pos + glm::vec3(1.f, 0.f, 0.f) * speed * dt);
        }

        if(is_key_pressed(GLFW_KEY_D))
        {
            // Mover en X negativo (asumiendo que 'D' es para ir a la izquierda)
            if(pos.x > -1.5f)
                object->getTransform()->translate(pos + glm::vec3(-1.f, 0.f, 0.f) * speed * dt);
        }
        
        // 3. --- Lógica de Movimiento y Reaparición de Obstáculos ---
        if (obstacles_list && scene) 
        {
            for (unsigned id : *obstacles_list) 
            {
                std::shared_ptr<GameObject> obstacle = scene->at(id);
                glm::vec3 obs_pos = obstacle->getTransform()->getPosition();

                // Mover el obstáculo hacia el usuario (Z negativa)
                obs_pos.z -= OBSTACLE_SPEED * dt;
                
                // Verificar si el obstáculo alcanzó el límite de reaparición
                if (obs_pos.z < Z_LIMIT) 
                {
                    // Reiniciar la posición Z y asignar una X aleatoria (-1.0 a 1.0)
                    obs_pos.z = Z_RESET;
                    obs_pos.x = distrib_x(gen); 
                }

                // Aplicar la nueva posición
                obstacle->getTransform()->translate(obs_pos);
            }
        }
        // -------------------------------------------------------------
        
        std::cout << "delta: " << dt <<  ", FPS: " << 1.f / dt << std::endl;

    }

    ~inputManager() {}
};

int main()
{
    try
    {
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

        scene->at(user)->getTransform()->translate(glm::vec3(0.f, 0.f, -2.5f));
        
        camera->setFront(-20.f, 90.f);
        camera->setUp(glm::vec3(0.f, 1.f, 0.f));

        

        camera->init(glm::vec3(0.f, 1.5f, -2.5f), main_window->get_aspect_ratio(), 45.f, 0.1f, 100.f, true);
        camera->activate();
        
        //scene->at(user)->getTransform()->rotate(-20.f, glm::vec3(1.f, 0.f, 0.f));
       
        // Cargar modelo TV
        auto tv = scene->createGameObject();
        auto tv_model = scene->createModel(tv);
        if (tv_model->loadModel("crt-tv/source/model/model.dae"))
        {
            scene->at(tv)->getTransform()->translate(0.f, 0.f, -1.5f);
            scene->at(tv)->getTransform()->scale(0.5f, 0.5f, 0.5f);
            scene->at(tv)->setVisible(true);
        }
        
        // Restaurar modelos de prueba
        scene->createModel(user)->loadModel("suzanne.fbx");
        
        // Cargar esfera
        auto sphere = scene->createGameObject();
        scene->createModel(sphere)->loadModel("sphere.fbx");
        scene->at(sphere)->getTransform()->translate(2.f, 0.f, -3.f);
        scene->at(sphere)->getTransform()->scale(0.5f, 0.5f, 0.5f);
        scene->at(sphere)->setVisible(true);
        
        // Cargar dado (cubo)
        auto dado = scene->createGameObject();
        scene->createModel(dado)->loadModel("dado.fbx");
        scene->at(dado)->getTransform()->translate(-2.f, 0.f, -3.f);
        scene->at(dado)->getTransform()->scale(0.3f, 0.3f, 0.3f);
        scene->at(dado)->setVisible(true);
        
        // Cargar cilindro
        auto cilinder = scene->createGameObject();
        scene->createModel(cilinder)->loadModel("cilinder.fbx");
        scene->at(cilinder)->getTransform()->translate(0.f, 0.f, -4.f);
        scene->at(cilinder)->getTransform()->scale(0.4f, 0.4f, 0.4f);
        scene->at(cilinder)->setVisible(true);
        
        // Cargar mundo/terreno (usando sphere como base, o puedes usar otro modelo)
        auto mundo = scene->createGameObject();
        scene->createModel(mundo)->loadModel("sphere.fbx");
        scene->at(mundo)->getTransform()->translate(0.f, -2.f, 0.f);
        scene->at(mundo)->getTransform()->scale(10.f, 0.5f, 10.f); // Plano grande como terreno
        scene->at(mundo)->setVisible(true);

        auto obstacles = std::make_shared<std::vector<unsigned>>();


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


        // La escala de sphere ya se estableció arriba, solo escalar user
        scene->at(user)->getTransform()->scale(0.4f, 0.4f, 0.4f);

        obstacles->push_back(sphere);

        input->init(scene, scene->at(user), obstacles);

        auto renderer = std::make_shared<Renderer>();
        renderer->init();
        renderer->render(scene);

    }catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
