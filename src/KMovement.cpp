#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <limits> // Para el valor máximo (indicador de "no target")
#include <iostream>

#include "KMovement.hpp"
#include "GameObject.hpp"
#include "Body.hpp"

// Constante que indica que no hay posición objetivo (modo Velocity)
// El valor máximo es un buen indicador de "sin destino"
static constexpr float NO_TARGET_VALUE = std::numeric_limits<float>::max();

namespace Engine {

// --- Helper Functions ---
JPH::RVec3 KMovement::toJoltRVec3(const glm::vec3& v) const {
    return JPH::RVec3(v.x, v.y, v.z); 
}

glm::vec3 KMovement::toGlmVec3(const JPH::RVec3& v) const {
    return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
}

// --- Public Implementation ---

KMovement::KMovement(std::shared_ptr<GameObject> owner) 
    : m_owner(owner) 
{
    // Inicializar el target a un valor que nunca será una posición real (modo Velocity)
    m_targetPosition = glm::vec3(NO_TARGET_VALUE);
}

void KMovement::stop() {
    std::cout << "-------- STOP --------" << std::endl;
    m_isMoving = false;
    m_velocity = glm::vec3(0.0f);
    m_targetPosition = glm::vec3(NO_TARGET_VALUE); // Restablecer el target
    m_owner->getBody()->SetVelocity(JPH::Vec3::sZero());
}

void KMovement::setVelocity(const glm::vec3& velocity) {
    stop(); 
    m_velocity = velocity;
    
    if (glm::length2(m_velocity) > 0.0f) {
        m_isMoving = true;
        // Mantenemos el target en NO_TARGET_VALUE para indicar modo Velocity
    }
}

void KMovement::moveTo(const glm::vec3& targetPosition, float speed) {
    // 1. Limpiar estados anteriores
    stop(); 
    
    // 2. Guardar el destino
    m_targetPosition = targetPosition;

  
   auto body = m_owner->getBody();

    glm::vec3 currentPos = toGlmVec3(body->GetPosition());
    glm::vec3 direction = m_targetPosition - currentPos;
    
    if (glm::length2(direction) > 0.0f) {
        // Normalizar la dirección y multiplicar por la velocidad deseada
        m_velocity = glm::normalize(direction) * speed;
        m_isMoving = true;
    }
}

void KMovement::Update(const float& dt) {
    if (!m_isMoving) {
        return;
    }

    auto body = m_owner->getBody();

    // --- Lógica de Movimiento ---
    glm::vec3 currentPos = toGlmVec3(body->GetPosition());
    glm::vec3 newPos = currentPos + m_velocity * dt;
    bool isMoveToMode = m_targetPosition.x != NO_TARGET_VALUE;
    bool _stop = false;

    if (isMoveToMode) {
        // Lógica de interrupción del modo MoveTo

        // 1. Vector de desplazamiento REAL en este frame
        glm::vec3 actualDisplacement = newPos - currentPos;

        // 2. Vector del punto de inicio al objetivo
        glm::vec3 startToTarget = m_targetPosition - currentPos;

        // Utilizamos la proyección (dot product) para ver si nos hemos pasado del destino
        // Si el producto escalar entre (startToTarget) y (m_targetPosition - newPos) es negativo o cero, 
        // significa que el destino está detrás de nosotros o hemos llegado.
        glm::vec3 newPosToTarget = m_targetPosition - newPos;
        
        // Criterio de parada: Si el producto escalar cambia de signo, nos hemos pasado.
        // O si la distancia al cuadrado al target es menor que la distancia que se mueve en un frame.
        auto dot = glm::dot(startToTarget, newPosToTarget);

        std::cout << "dot: " << dot << std::endl;

        if (dot <= 0.0f) {
            // Nos hemos pasado o estamos exactamente en el destino.
            newPos = m_targetPosition; // Asegurar posición final exacta
            _stop = true;
        }
    }

    std::cout << "newPos: " << toJoltRVec3(newPos) << std::endl;

    // --- Aplicación del Movimiento Cinemático ---
    // Si stop() fue llamado, el movimiento ya terminó y 'newPos' es igual a 'm_targetPosition'.
    body->Move(
        toJoltRVec3(newPos), 
        JPH::Quat::sIdentity(), 
        dt
    );

    if(_stop)
        stop();

}

}