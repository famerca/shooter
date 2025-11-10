#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace Engine {

class Physics;

class Body {
public:
    Body() = default;
    explicit Body(JPH::BodyID id, bool isDynamic);
    ~Body();

    // Prohíbe copias, pero permite movimiento
    Body(const Body&) = delete;
    Body& operator=(const Body&) = delete;
    Body(Body&& other) noexcept;
    Body& operator=(Body&& other) noexcept;

    void destroy();

    void ApplyImpulse(const JPH::Vec3& impulse);
    void ApplyForce(const JPH::Vec3& force);
    JPH::RVec3 GetPosition() const;

    bool IsValid() const { return ! m_BodyID.IsInvalid(); }

private:
    friend class Physics;

    JPH::BodyID m_BodyID;
    bool m_IsDynamic = false;
};

} // namespace Engine
