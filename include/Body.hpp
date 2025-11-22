#pragma once
#include <memory>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/EActivation.h>
#include <glm/glm.hpp>

#ifndef BODY_H
#define BODY_H

class GameObject;

namespace Engine {


enum class BodyType {
    Static,
    Dynamic,
    Kinematic // ¡El nuevo tipo!
};

class Physics;
class Listener;

class Body {
public:
    Body() = default;
    explicit Body(JPH::BodyID id, BodyType type);
    ~Body();

    // Prohíbe copias, pero permite movimiento
    Body(const Body&) = delete;
    Body& operator=(const Body&) = delete;
    Body(Body&& other) noexcept;
    Body& operator=(Body&& other) noexcept;

    
    void destroy();
    
    void ApplyImpulse(const JPH::Vec3& impulse);
    void ApplyForce(const JPH::Vec3& force);
    void serVelocity(const JPH::Vec3& velocity);
    JPH::RVec3 GetPosition() const;
    void SetPosition(const glm::vec3& inPosition, JPH::EActivation inActivation = JPH::EActivation::Activate);
    void SetVelocity(const JPH::Vec3& velocity);
    void SetOwner(std::shared_ptr<GameObject> gameObject);

    void Move(const JPH::Vec3& position, const JPH::Quat& rotation, float delta);

    const BodyType &getType() const { return m_type; }

    void update();

    bool IsValid() const { return ! m_BodyID.IsInvalid(); }

private:
    friend class Physics;
    friend class Listener;

    JPH::BodyID m_BodyID;
    BodyType m_type = BodyType::Static;
    std::shared_ptr<GameObject> owner;
    
};

} // namespace Engine


#endif // JPH_BODY_H