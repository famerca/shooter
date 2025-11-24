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
                        std::shared_ptr<GameObject> usr)
{
    this->user = usr;
    this->scene = scene_ptr;
}

void inputManager::update(const float &dt) noexcept 
{
    // Protección básica: si no hay objeto o escena, no hacemos nada
    if (!user || !scene) return;

    // 1. --- Lógica del Salto del Jugador (Y) ---
    if (is_key_pressed(GLFW_KEY_SPACE) && !is_jumping)
    {
        if (auto body = user->getBody()) {
            body->ApplyImpulse({0.f, 1000.f, 0.f});
            is_jumping = true;
        }
    }

    if(is_key_pressed(GLFW_KEY_LEFT_SHIFT))
    {
        if (auto body = user->getBody()) {
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
    if (auto body = user->getBody()) {
        if(is_key_pressed(GLFW_KEY_A)) body->ApplyImpulse({30.f, 0.f, 0.f});
        if(is_key_pressed(GLFW_KEY_D)) body->ApplyImpulse({-30.f, 0.f, 0.f});
        if(is_key_pressed(GLFW_KEY_W)) body->ApplyImpulse({0.f, 0.f, 40.f});
        if(is_key_pressed(GLFW_KEY_S)) body->ApplyImpulse({0.f, 0.f, -40.f});
    }
}