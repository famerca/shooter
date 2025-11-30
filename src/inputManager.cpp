#include <GLS/TransformComponent.hpp> // Necesario para getTransform()
#include <GLS/Body.hpp>       // Necesario para getBody()
#include <GLS/CameraComponent.hpp> // Necesario para getCamera()

#include <GLFW/glfw3.h>   // Para los códigos de teclas (GLFW_KEY_...)
#include <glm/glm.hpp>
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

void inputManager::handle_move() noexcept
{
    if (auto body = user->getBody()) {
        
        auto camera = scene->getCamera();
        if (camera) {
            // A. OBTENER VECTORES DE LA CÁMARA
            // Asumo que tu motor devuelve un vector {x, y, z}. 
            // Si devuelve GLM o similar, la lógica es la misma.
            glm::vec3 camForward = camera->getForward(); // Necesitas este vector
            glm::vec3 camRight   = camera->getRight();   // Y este vector
            
            // B. APLANAR EN Y (Proyectar al suelo)
            // Importante para que el personaje no intente volar al mirar arriba
            camForward.y = 0.0f;
            camRight.y   = 0.0f;

            // C. NORMALIZAR
            // (Necesitas una función normalize, asumo que usas alguna librería math o GLM)
            // Si usas Jolt, JPH::Vec3 tiene .Normalized()
            // Aquí uso pseudocódigo matemático para que lo adaptes a tu librería de vec3
            camForward = glm::normalize(camForward);
            camRight   = glm::normalize(camRight);

            // D. SUMAR INPUTS
            // Creamos un vector de dirección final
            float moveX = 0.0f;
            float moveZ = 0.0f;
            float impulseMagnitude = 40.0f; // Tu fuerza original

            // Vector acumulado
            // Nota: Uso Jolt Vec3 como referencia de estructura
            float dirX = 0.0f;
            float dirZ = 0.0f;

            if(is_key_pressed(GLFW_KEY_W)) {
                dirX += camForward.x;
                dirZ += camForward.z;
            }
            if(is_key_pressed(GLFW_KEY_S)) {
                dirX -= camForward.x;
                dirZ -= camForward.z;
            }
            // Verifica si en tu motor Right es + o - según tu sistema de coordenadas
            if(is_key_pressed(GLFW_KEY_D)) { 
                dirX += camRight.x;
                dirZ += camRight.z;
            }
            if(is_key_pressed(GLFW_KEY_A)) {
                dirX -= camRight.x;
                dirZ -= camRight.z;
            }

            // E. APLICAR IMPULSO
            // Solo aplicamos si hay movimiento para evitar dividir por cero al normalizar
            if (dirX != 0.0f || dirZ != 0.0f) {
                // Normalizamos el vector resultante para evitar que moverse en diagonal sea más rápido
                float length = std::sqrt(dirX * dirX + dirZ * dirZ);
                dirX /= length;
                dirZ /= length;

                // Aplicamos la fuerza
                body->ApplyImpulse({dirX * impulseMagnitude, 0.f, dirZ * impulseMagnitude});
            }
        }
    }

}