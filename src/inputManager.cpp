#include <GLS/TransformComponent.hpp> // Necesario para getTransform()
#include <GLS/Body.hpp>       // Necesario para getBody()
#include <GLS/CameraComponent.hpp> // Necesario para getCamera()

#include <GLFW/glfw3.h>   // Para los códigos de teclas (GLFW_KEY_...)
#include <iostream>       // Si usas cout

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
                        std::shared_ptr<GameObject> object_ptr, 
                        std::shared_ptr<std::vector<unsigned>> obstacles)
{
    this->object = object_ptr;
    this->scene = scene_ptr;
    this->obstacles_list = obstacles;
}

void inputManager::update(const float &dt) noexcept 
{
    // Protección básica: si no hay objeto o escena, no hacemos nada
    if (!object || !scene) return;

    // glm::vec3 pos = object->getTransform()->getPosition(); // (Variable no usada en el snippet original, pero la dejo por si acaso)

    // 1. --- Lógica del Salto del Jugador (Y) ---
    if (is_key_pressed(GLFW_KEY_SPACE) && !is_jumping)
    {
        if (auto body = object->getBody()) {
            body->ApplyImpulse({0.f, 1000.f, 0.f});
            is_jumping = true;
        }
    }

    if(is_key_pressed(GLFW_KEY_LEFT_SHIFT))
    {
        if (auto body = object->getBody()) {
            body->ApplyImpulse({0.f, -10.f, 0.f});
        }
    }

    // --- Control de Cámara ---
    if (auto cam = scene->getCamera()) {
        if(is_key_pressed(GLFW_KEY_LEFT))  cam->rotate(-50.f, 0.f);
        if(is_key_pressed(GLFW_KEY_RIGHT)) cam->rotate(50.f, 0.f);
        if(is_key_pressed(GLFW_KEY_UP))    cam->rotate(0.f, 50.f);
        if(is_key_pressed(GLFW_KEY_DOWN))  cam->rotate(0.f, -50.f);
    }

    // 2. --- Lógica de Movimiento Horizontal del Jugador (X/Z) ---
    if (auto body = object->getBody()) {
        if(is_key_pressed(GLFW_KEY_A)) body->ApplyImpulse({30.f, 0.f, 0.f});
        if(is_key_pressed(GLFW_KEY_D)) body->ApplyImpulse({-30.f, 0.f, 0.f});
        if(is_key_pressed(GLFW_KEY_W)) body->ApplyImpulse({0.f, 0.f, 40.f});
        if(is_key_pressed(GLFW_KEY_S)) body->ApplyImpulse({0.f, 0.f, -40.f});
    }
    
    // 3. --- Lógica de Obstáculos (Comentada tal cual el original) ---
    /*
    if (obstacles_list && scene) 
    {
         for (unsigned id : *obstacles_list) 
         {
             std::shared_ptr<GameObject> obstacle = scene->at(id);
             if (!obstacle) continue;

             auto transform = obstacle->getTransform();
             glm::vec3 obs_pos = transform->getPosition();

             // Mover hacia el usuario
             obs_pos.z -= OBSTACLE_SPEED * dt;
            
             // Reinicio
             if (obs_pos.z < Z_LIMIT) 
             {
                 obs_pos.z = Z_RESET;
                 obs_pos.x = distrib_x(gen); 
             }

             transform->translate(obs_pos); // Nota: translate suele ser relativo, revisa si necesitas setPosition
         }
    }
    */
}