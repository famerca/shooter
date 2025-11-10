#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/RegisterTypes.h>
#include <memory>
#include <vector>
#include "Body.hpp"

JPH_SUPPRESS_WARNINGS

namespace Engine {

class Physics {
public:
    static Physics& Get();

    void Init();
    void Shutdown();
    void Step(float deltaTime);

    // Creadores de cuerpos
    std::shared_ptr<Body> CreateBox(const JPH::Vec3& halfExtent, const JPH::RVec3& pos, bool isDynamic);
    std::shared_ptr<Body> CreateSphere(float radius, const JPH::RVec3& pos, bool isDynamic);

    // Acceso a interfaces internas
    JPH::PhysicsSystem& GetSystem() { return *m_PhysicsSystem; }
    JPH::BodyInterface& GetBodyInterface() { return m_PhysicsSystem->GetBodyInterface(); }

private:
    Physics() = default;

    // Filtros internos
    class BPLayerInterfaceImpl;
    class ObjectVsBroadPhaseLayerFilterImpl;
    class ObjectLayerPairFilterImpl;

    std::vector<std::shared_ptr<Body>> m_Bodies;
    std::unique_ptr<JPH::PhysicsSystem> m_PhysicsSystem;
    std::unique_ptr<JPH::TempAllocatorImpl> m_TempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> m_JobSystem;
    std::unique_ptr<BPLayerInterfaceImpl> m_BPLayerInterface;
    std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> m_ObjectVsBroadPhaseFilter;
    std::unique_ptr<ObjectLayerPairFilterImpl> m_ObjectPairFilter;
};

} // namespace Engine
