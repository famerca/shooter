#include "Body.hpp"
#include "Physics.hpp"
#include <iostream>
using namespace JPH;

namespace Engine {

Body::Body(Body&& other) noexcept {
    m_BodyID = other.m_BodyID;
    m_IsDynamic = other.m_IsDynamic;
    other.m_BodyID = BodyID(); // invalida el antiguo
}

Body& Body::operator=(Body&& other) noexcept {
    if (this != &other) {
        m_BodyID = other.m_BodyID;
        m_IsDynamic = other.m_IsDynamic;
        other.m_BodyID = BodyID();
    }
    return *this;
}

Body::Body(BodyID id, bool isDynamic)
    : m_BodyID(id), m_IsDynamic(isDynamic) {}

void Body::destroy() {
    if (IsValid()) {
        auto& iface = Physics::Get().GetBodyInterface();
        iface.RemoveBody(m_BodyID);
        iface.DestroyBody(m_BodyID);
    }
}

Body::~Body() {

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
