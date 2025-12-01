#include <iostream>       // Si usas cout

#include <GLS/TransformComponent.hpp> // Necesario para getTransform()
#include <GLS/Body.hpp>       // Necesario para getBody()
#include <GLS/CameraComponent.hpp> // Necesario para getCamera()
#include <GLS/UIManager.hpp> // Para manejar pausa

#include <GLFW/glfw3.h>   // Para los códigos de teclas (GLFW_KEY_...)
#include <iostream>       // Si usas cout
#include <GLS/Utils.hpp>

#include "inputManager.hpp"


using namespace Engine;

inputManager::inputManager() : Input() 
{
    // Inicialización del generador aleatorio
    std::random_device rd;
    gen = std::mt19937(rd());
    distrib_x = std::uniform_real_distribution<>(-1.0, 1.0);
}

inputManager::~inputManager() 
{
    // Destructor vacío (o limpieza si fuera necesaria)
}

void inputManager::init(std::shared_ptr<Scene> scene_ptr, 
                        std::shared_ptr<GameObject> usr,
                        std::shared_ptr<UIManager> ui_mgr)
{
    this->user = usr;
    this->scene = scene_ptr;
    this->ui_manager = ui_mgr; // Guarda el UIManager para manejar pausa
}

void inputManager::update(const float &dt) noexcept 
{
    // Protección básica: si no hay objeto o escena, no hacemos nada
    if (!user || !scene) return;

        // Manejar pausa con ESC (lógica del juego, no del motor)
    if(is_key_pressed(GLFW_KEY_ESCAPE))
    {
        handlePauseInput();
    }
    else
    {
        last_esc_state = false;
    }

    // 1. --- Lógica del Salto del Jugador (Y) ---
    if (is_key_pressed(GLFW_KEY_SPACE) && !is_jumping)
    {
        if (auto body = user->getBody()) {
            body->ApplyImpulse({0.f, 1800.f, 0.f});
            is_jumping = true;
        }
    }

    handle_camera(dt);

    handle_move();

    gameOver();
    
}

void inputManager::handle_camera(const float &dt) noexcept
{
    MousePair mouse = get_mouse_delta();
    if (mouse.x != 0.f || mouse.y != 0.f)
    {
        if (auto cam = scene->getCamera()) {
            cam->rotate(mouse.x * sensitivity * 1000 * dt, mouse.y * sensitivity * 1000 * dt);
        }
    }
}

void inputManager::setJumping(bool jumping) noexcept
{
    is_jumping = jumping;
}


void inputManager::gameOver() noexcept
{
    if (auto body = user->getBody())
    {
       auto pos = body->GetPosition();
    
       if(pos.GetY() < -10)
       {
           if(onGameOver)
           {
               onGameOver();
           }
       }
    }
}

void inputManager::setOnGameOver(Engine::Listener::Callback callback) noexcept
{
    onGameOver = callback;
}

void inputManager::handlePauseInput() noexcept
{
    if (!ui_manager || !ui_manager->IsInitialized())
        return;
    
    // Detectar transición de false a true (tecla presionada)
    if (!last_esc_state)
    {
        // Si el menú principal está visible, no hacer nada (ya está pausado)
        if (!ui_manager->IsTemplateVisible("main_menu"))
        {
            // Si el menú de pausa está visible, ocultarlo (reanudar)
            if(ui_manager->IsTemplateVisible("pause_menu"))
            {
                ui_manager->HideTemplate("pause_menu");
            }
            // Si no está visible, mostrarlo (pausar)
            else
            {
                ui_manager->ShowTemplate("pause_menu");
            }
        }
    }
    
    last_esc_state = true;
}

void inputManager::handle_move() noexcept
{
    if (auto body = user->getBody()) {
        
        auto camera = scene->getCamera();
        if (camera) {
            // A. OBTENER VECTORES DE LA CÁMARA

            glm::vec3 camForward = camera->getForward(); // Necesitas este vector
            glm::vec3 camRight   = camera->getRight();   // Y este vector

            std::cout << "[InputManager] camForward: " << Utils::toJoltVec3(camForward) << std::endl;
            std::cout << "[InputManager] camRight: " << Utils::toJoltVec3(camRight) << std::endl;


            glm::vec3 impulse = glm::vec3(0.f, 0.f, 0.f);
            

            camForward = glm::normalize(camForward);
            camRight   = glm::normalize(camRight);


            if(is_key_pressed(GLFW_KEY_W)) {
                impulse += camForward * impulseMagnitude;
                std::cout << "[InputManager] impulse: " << Utils::toJoltVec3(impulse) << std::endl;
            }   
            if(is_key_pressed(GLFW_KEY_S)) {
                impulse -= camForward * impulseMagnitude;
            }
            // Verifica si en tu motor Right es + o - según tu sistema de coordenadas
            if(is_key_pressed(GLFW_KEY_D)) { 
                impulse += camRight * impulseMagnitude;
            }
            if(is_key_pressed(GLFW_KEY_A)) {
                 impulse -= camRight * impulseMagnitude;
            }
                                               
            body->ApplyImpulse({impulse.x, 0.f, impulse.z});
            
        }
    }

}