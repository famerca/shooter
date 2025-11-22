#include "Body.hpp"
#include "Physics.hpp"
#include "GameObject.hpp"
#include <iostream>

using namespace JPH;

namespace Engine {

Body::Body(Body&& other) noexcept {
    m_BodyID = other.m_BodyID;
    m_type = other.m_type;
    other.m_BodyID = BodyID(); // invalida el antiguo
    owner = other.owner;
}

Body& Body::operator=(Body&& other) noexcept {
    if (this != &other) {
        m_BodyID = other.m_BodyID;
        m_type = other.m_type;
        other.m_BodyID = BodyID();
        owner = other.owner;
    }
    return *this;
}

Body::Body(BodyID id, BodyType type)
    : m_BodyID(id), m_type(type), owner{nullptr}
    {}

void Body::destroy() {
    if (IsValid()) {
        auto& iface = Physics::Get().GetBodyInterface();
        iface.RemoveBody(m_BodyID);
        iface.DestroyBody(m_BodyID);
    }
}

Body::~Body() {

}


void Body::SetOwner(std::shared_ptr<GameObject> gameObject)
{
    owner = gameObject;
}

void Body::update()
{
    if (owner != nullptr && IsValid())
    {
        JPH::RVec3 pos = Physics::Get().GetBodyInterface().GetCenterOfMassPosition(m_BodyID);
        JPH::Quat jolt_rot = Physics::Get().GetBodyInterface().GetRotation(m_BodyID);

        //std::cout << pos << std::endl;
        owner->getTransform()->rotate( jolt_rot.GetW(), jolt_rot.GetX(), jolt_rot.GetY(), jolt_rot.GetZ());
        owner->getTransform()->translate(pos.GetX(), pos.GetY(), pos.GetZ());
    }
}

void Body::SetPosition(const glm::vec3& inPosition, JPH::EActivation inActivation) {
    if (IsValid()) {

        BodyInterface& iface = Physics::Get().GetBodyInterface();
        // --- CONVERSIÓN CRUCIAL ---
        Vec3 jolt_vec3(inPosition.x, inPosition.y, inPosition.z);
        iface.SetPosition(m_BodyID, jolt_vec3, inActivation);

    }
}

void Body::SetVelocity(const Vec3& velocity)
{
    if (IsValid()) {
        BodyInterface& iface = Physics::Get().GetBodyInterface();
        iface.SetLinearVelocity(m_BodyID, velocity);
    }
}


void Body::ApplyImpulse(const Vec3& impulse) {
    if (m_type == BodyType::Dynamic)
    {
        //std::cout << "IMPULSE: " << impulse << std::endl;
        Physics::Get().GetBodyInterface().AddImpulse(m_BodyID, impulse);
    }
}

void Body::serVelocity(const Vec3& velocity) {
    if (m_IsDynamic)
        Physics::Get().GetBodyInterface().SetLinearVelocity(m_BodyID, velocity);
}

void Body::ApplyForce(const Vec3& force) {
    if (m_type == BodyType::Dynamic)
        Physics::Get().GetBodyInterface().AddForce(m_BodyID, force);
}

void Body::Move(const Vec3& position, const Quat& rotation, float delta) {
    if (IsValid() && m_type == BodyType::Kinematic) {

        BodyInterface& iface = Physics::Get().GetBodyInterface();

        iface.MoveKinematic(
            m_BodyID,
            position,
            rotation,
            delta
        );
    
    
        iface.ActivateBody(m_BodyID);
    // Los cuerpos cinemáticos deben estar activos para moverse y colisionar.

    }
}

RVec3 Body::GetPosition() const {
    if (!IsValid()) return RVec3::sZero();
    return Physics::Get().GetBodyInterface().GetCenterOfMassPosition(m_BodyID);
}

} // namespace Engine
