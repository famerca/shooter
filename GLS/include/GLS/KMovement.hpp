#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <glm/glm.hpp>
#include <memory>

#ifndef KMOVEMENT_HPP
#define KMOVEMENT_HPP


namespace Engine {
    
class GameObject; 
class Body; 

class KMovement {
public:
    KMovement(std::shared_ptr<GameObject> owner);

    void Update(const float& dt);

    // --- Métodos de Movimiento ---

    // 1. Movimiento indefinido: Establece el vector de velocidad directamente.
    void setVelocity(const glm::vec3& velocity);
    
    // 2. Movimiento a un punto: Calcula el vector de velocidad inicial y se mueve hasta llegar.
    void moveTo(const glm::vec3& targetPosition, float speed);

    // 3. Detiene cualquier movimiento en curso.
    void stop();

private:
    std::shared_ptr<GameObject> m_owner;
    
    // Estado único de movimiento
    bool m_isMoving = false; // Indica si el Update debe hacer algo.

    // Mecanismo de movimiento
    glm::vec3 m_velocity = glm::vec3(0.0f); // Velocidad actual (vector)

    // Mecanismo de interrupción (Usado para el modo MoveTo)
    // Si este vector es diferente de cero, significa que estamos en modo MoveTo
    glm::vec3 m_targetPosition = glm::vec3(0.0f);
    
    // --- Funciones Auxiliares ---
    JPH::RVec3 toJoltRVec3(const glm::vec3& v) const;
    glm::vec3 toGlmVec3(const JPH::RVec3& v) const;
};

} // namespace 

#endif // KMOVEMENT_HPP