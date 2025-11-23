#pragma once

#include <memory>
#include <vector>
#include <random>
#include <GLS/Input.hpp> // Asumiendo que la clase base Input está aquí
#include <GLS/GameObject.hpp>
#include <GLS/Scene.hpp>


// --- FORWARD DECLARATIONS ---
// Esto es CRUCIAL: Le decimos al compilador "Existen estas clases", 
// pero no incluimos sus archivos .h pesados aquí.


class inputManager : public Engine::Input
{
private:
    // Atributos del Jugador
    std::shared_ptr<Engine::GameObject> object{nullptr};
    std::shared_ptr<Engine::Scene> scene{nullptr};
    float sensitivity{1.f};
    float speed{3.f}; 

    // Atributos de Salto
    float jump_velocity{10.f}; 
    float gravity{-20.f};      
    float current_y_velocity{0.f}; 
    float initial_y{0.f};      

    // Atributos de Obstáculos
    std::shared_ptr<std::vector<unsigned>> obstacles_list{nullptr}; 
    const float OBSTACLE_SPEED = 3.0f;
    const float Z_RESET = 5.0f;       
    const float Z_LIMIT = -6.0f;      
    
    // Generador aleatorio
    std::mt19937 gen;
    std::uniform_real_distribution<> distrib_x;

public:
    bool is_jumping {false};

    // Constructor y Destructor
    inputManager();
    ~inputManager();

    // Inicialización
    void init(std::shared_ptr<Engine::Scene> scene, 
              std::shared_ptr<Engine::GameObject> object, 
              std::shared_ptr<std::vector<unsigned>> obstacles);

    // Loop principal
    void update(const float &dt) noexcept;
};