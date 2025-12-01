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

inputManager::inputManager(const bool& paused) : Input(), paused(paused)
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
                        std::shared_ptr<GameObject> usr)
{
    this->user = usr;
    this->scene = scene_ptr;
}

JPH::Vec3 inputManager::getForward() const noexcept
{
    return Engine::Utils::toJoltVec3(scene->getCamera()->getForward());
}

void inputManager::update(const float &dt) noexcept 
{
    // Protección básica: si no hay objeto o escena, no hacemos nada
    if (!user || !scene) return;

    // Manejar pausa con ESC (lógica del juego, no del motor)
    handlePauseInput();

    if(paused)
        return;

    // 1. --- Lógica del Salto del Jugador (Y) ---
    if (is_key_pressed(GLFW_KEY_SPACE) && !is_jumping)
    {
        if (auto body = user->getBody()) {
            body->ApplyImpulse({0.f, 1800.f, 0.f});
            is_jumping = true;
        }
    }

    if(is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        holing = true;
    }else if(holing)
    {
        holing = false;
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

const bool & inputManager::is_holding() const noexcept
{
    return holing;
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
    // Detectar transición de false a true (tecla presionada)
    if (is_key_pressed(GLFW_KEY_ESCAPE) && !last_esc_state)
    {
        // Si el menú principal está visible, no hacer nada (ya está pausado)
        if(onPause)
            onPause();

        last_esc_state = true;
    }else if(!is_key_pressed(GLFW_KEY_ESCAPE)) 
    {
        last_esc_state = false;
    }
    
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

void inputManager::setOnPause(Engine::Listener::Callback callback) noexcept
{
    onPause = callback;
}