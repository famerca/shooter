#include "Body.hpp"
#include "Physics.hpp"
#include "GameObject.hpp"

using namespace JPH;

namespace Engine {

Body::Body(Body&& other) noexcept {
    m_BodyID = other.m_BodyID;
    m_IsDynamic = other.m_IsDynamic;
    other.m_BodyID = BodyID(); // invalida el antiguo
    owner = other.owner;
}

Body& Body::operator=(Body&& other) noexcept {
    if (this != &other) {
        m_BodyID = other.m_BodyID;
        m_IsDynamic = other.m_IsDynamic;
        other.m_BodyID = BodyID();
        owner = other.owner;
    }
    return *this;
}

Body::Body(BodyID id, bool isDynamic)
    : m_BodyID(id), m_IsDynamic(isDynamic), owner{nullptr}
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


void Body::ApplyImpulse(const Vec3& impulse) {
    if (m_IsDynamic)
        Physics::Get().GetBodyInterface().AddImpulse(m_BodyID, impulse);
}

void Body::ApplyForce(const Vec3& force) {
    if (m_IsDynamic)
        Physics::Get().GetBodyInterface().AddForce(m_BodyID, force);
}

RVec3 Body::GetPosition() const {
    if (!IsValid()) return RVec3::sZero();
    return Physics::Get().GetBodyInterface().GetCenterOfMassPosition(m_BodyID);
}

} // namespace Engine
