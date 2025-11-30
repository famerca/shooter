#pragma once

#include <memory>
#include <vector>
#include <random>
#include <GLS/Input.hpp> // Asumiendo que la clase base Input está aquí
#include <GLS/GameObject.hpp>
#include <GLS/Scene.hpp>
#include <GLS/Listener.hpp>


// --- FORWARD DECLARATIONS ---
// Esto es CRUCIAL: Le decimos al compilador "Existen estas clases", 
// pero no incluimos sus archivos .h pesados aquí.


class inputManager : public Engine::Input
{
private:
    // Atributos del Jugador
    std::shared_ptr<Engine::GameObject> user{nullptr};
    std::shared_ptr<Engine::Scene> scene{nullptr};
    float sensitivity{0.5f};
    bool is_jumping {false};
    Engine::Listener::Callback onGameOver;
  

    // Atributos de Obstáculos
    
    // Generador aleatorio
    std::mt19937 gen;
    std::uniform_real_distribution<> distrib_x;

public:
    

    // Constructor y Destructor
    inputManager();
    ~inputManager();

    // Inicialización
    void init(std::shared_ptr<Engine::Scene> scene, 
              std::shared_ptr<Engine::GameObject> object);

    // Loop principal
    void update(const float &dt) noexcept;

    void handle_camera (const float &dt) noexcept;
    void handle_move() noexcept;

    void gameOver() noexcept;

    void setJumping(bool) noexcept;

    void setOnGameOver(Engine::Listener::Callback callback) noexcept;
};