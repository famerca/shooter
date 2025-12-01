#include "Body.hpp"
#include "Physics.hpp"
#include "GameObject.hpp"
#include <iostream>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>

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
    {
        m_constraint = nullptr;
    }

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


void Body::SetRotation(const glm::quat& rotation)
{
    if (IsValid())
    {
        JPH::Quat jolt_rot(rotation.x, rotation.y, rotation.z, rotation.w);
        Physics::Get().GetBodyInterface().SetRotation(m_BodyID, jolt_rot, JPH::EActivation::DontActivate);
    }
}

void Body::SetShape(const JPH::Vec3& halfExtent)
{
    if (IsValid())
    {
        auto& iface = Physics::Get().GetBodyInterface();
        JPH::BoxShapeSettings boxSettings(halfExtent);
        JPH::ShapeSettings::ShapeResult result = boxSettings.Create();
    }
}

void Body::ApplyImpulse(const Vec3& impulse) {
    if (m_type == BodyType::Dynamic)
    {
        //std::cout << "IMPULSE: " << impulse << std::endl;
        Physics::Get().GetBodyInterface().AddImpulse(m_BodyID, impulse);
    }
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


void Body::constraintRotation(std::shared_ptr<Body> static_world_body_ref)
{
    if (IsValid())
    {
        auto &system = Physics::Get().GetSystem();
        auto &body_interface = Physics::Get().GetBodyInterface();
        Vec3 position = Physics::Get().GetBodyInterface().GetCenterOfMassPosition(m_BodyID);
        SixDOFConstraintSettings settings;
        // 1. Establecer el Espacio de Restricción
        // El modo más sencillo para este caso es usar el espacio global (WorldSpace)
        // para que los ejes X, Y, Z de la restricción coincidan con los ejes globales.
        settings.mSpace = EConstraintSpace::WorldSpace;

        // 2. Definir la Traslación (T-D.O.F.) - Dejar libre
        // Usando la función de utilidad MakeFreeAxis()

        settings.MakeFreeAxis(SixDOFConstraintSettings::TranslationX);
        settings.MakeFreeAxis(SixDOFConstraintSettings::TranslationY);
        settings.MakeFreeAxis(SixDOFConstraintSettings::TranslationZ);

        // 3. Definir la Rotación (R-D.O.F.) - Restringir a Y
        // Usando la función de utilidad MakeFixedAxis() y MakeFreeAxis()

        // Bloquear rotación alrededor de X y Z (Impide inclinación y balanceo)
        settings.MakeFixedAxis(SixDOFConstraintSettings::RotationX);
        settings.MakeFixedAxis(SixDOFConstraintSettings::RotationZ);
        settings.MakeFixedAxis(SixDOFConstraintSettings::RotationY);

        // 4. (Opcional) Definir la Posición del Pivote
        // Aunque la traslación es libre, la posición de la restricción debe definirse.
        // Al usar WorldSpace, las posiciones 1 y 2 son las coordenadas del pivote.
        // Colocaremos el pivote en la posición inicial del cuerpo, aunque esto solo
        // define el origen del marco de referencia (los límites de traslación están libres).
        settings.mPosition1 = position;
        settings.mPosition2 = position; // Ambos deben coincidir en WorldSpace

        m_constraint = body_interface.CreateConstraint(
            &settings,
            static_world_body_ref->m_BodyID, // BodyID 1: El mundo estático
            m_BodyID    // BodyID 2: Tu objeto dinámico
        );

        if (m_constraint == nullptr)
        {
            std::cerr << "Error: No se pudo crear la restricción." << std::endl;
            return;
        }

        //5. añadir la restricción al sistema de restricciones
        system.AddConstraint(m_constraint);

        //6. Activar la restricción
        body_interface.ActivateConstraint(m_constraint);
            
    }
}


// JPH::Body* Body::getBody(JPH::BodyID ID)
// {

// }

} // namespace Engine
