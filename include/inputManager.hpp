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
  

    // Atributos de Obstáculos
    
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
              std::shared_ptr<Engine::GameObject> object);

    // Loop principal
    void update(const float &dt) noexcept;
};