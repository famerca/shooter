#include "Physics.hpp"
#include "Listener.hpp"
#include <iostream>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

using namespace JPH;

bool Engine::Physics::s_IsInitialized = false;

namespace Engine {


// --- Definición de capas ---
namespace Layers {
    static constexpr ObjectLayer NON_MOVING = 0;
    static constexpr ObjectLayer MOVING = 1;
    static constexpr ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};


// --- Implementaciones internas ---
class Physics::BPLayerInterfaceImpl : public BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() {
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayer(0);
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayer(1);
    }
    uint GetNumBroadPhaseLayers() const override { return 2; }
    BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }
    #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char * GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
        {
            switch ((BroadPhaseLayer::Type)inLayer)
            {
            case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
            case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
            default:													JPH_ASSERT(false); return "INVALID";
            }
        }
    #endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED
  
private:
    BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class Physics::ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override {
        if (inLayer1 == Layers::NON_MOVING) return inLayer2 == BroadPhaseLayer(1);
        return true;
    }
};

class Physics::ObjectLayerPairFilterImpl : public ObjectLayerPairFilter {
public:
    bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override {
        if (inObject1 == Layers::NON_MOVING) return inObject2 == Layers::MOVING;
        return true;
    }
};

// --- Singleton ---
Physics& Physics::Get() {
    static Physics instance;
    return instance;
}

bool Physics::IsInitialized() {
    return s_IsInitialized;
}

// --- Inicialización ---
void Physics::Init() {
    RegisterDefaultAllocator();
    Factory::sInstance = new Factory();
    RegisterTypes();

    m_TempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);
    m_JobSystem = std::make_unique<JobSystemThreadPool>(
        cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1
    );

    m_BPLayerInterface = std::make_unique<BPLayerInterfaceImpl>();
    m_ObjectVsBroadPhaseFilter = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    m_ObjectPairFilter = std::make_unique<ObjectLayerPairFilterImpl>();

    m_PhysicsSystem = std::make_unique<PhysicsSystem>();
    m_PhysicsSystem->Init(
        1024, //Max num bodies
        0, // Max num Mutex
        1024,
        1024,
        *m_BPLayerInterface,
        *m_ObjectVsBroadPhaseFilter,
        *m_ObjectPairFilter
    );

    //m_PhysicsSystem->SetGravity(Vec3(0.f, 0.f, 0.f));

    s_IsInitialized = true;

    m_PhysicsSystem->SetContactListener(&Listener::Get());

    std::cout << "[Physics] Initialized.\n";
}

// --- Step ---
void Physics::Step(float deltaTime) {
    if (m_PhysicsSystem)
        m_PhysicsSystem->Update(deltaTime, 1, m_TempAllocator.get(), m_JobSystem.get());
}

// --- Shutdown ---
void Physics::Shutdown() {
    if (m_PhysicsSystem) {
        for (auto& body : m_Bodies) {
            body->destroy();
        }
        UnregisterTypes();
        delete Factory::sInstance;
        Factory::sInstance = nullptr;
        m_PhysicsSystem.reset();
        std::cout << "[Physics] Shutdown.\n";
        s_IsInitialized = false;
    }
}

// --- Creación de cuerpos ---
std::shared_ptr<Body> Physics::CreateBox(const Vec3& halfExtent, const RVec3& pos, BodyType type) {
    // 1. Mapeo del BodyType a EMotionType de Jolt
    EMotionType motion;
    ObjectLayer layer;
    EActivation activation = EActivation::DontActivate; // Default

    switch (type) {
        case BodyType::Static:
            motion = EMotionType::Static;
            layer = Layers::NON_MOVING;
            break;
        case BodyType::Dynamic:
            motion = EMotionType::Dynamic;
            layer = Layers::MOVING;
            activation = EActivation::Activate; // Los cuerpos dinámicos suelen activarse
            break;
        case BodyType::Kinematic:
            motion = EMotionType::Kinematic;
            layer = Layers::MOVING; // Los cinemáticos interactúan como los dinámicos
            activation = EActivation::Activate; // Los cinemáticos siempre están "activos"
            break;
    }
    
    auto& iface = GetBodyInterface();
    BodyCreationSettings settings(
        new BoxShape(halfExtent),
        pos,
        Quat::sIdentity(),
        motion,
        layer
    );
    BodyID id = iface.CreateAndAddBody(settings, activation);
    std::shared_ptr<Body> body = std::make_shared<Body>(id, type);
    m_Bodies.push_back(body);
    return body;
}

std::shared_ptr<Body> Physics::CreateSphere(float radius, const RVec3& pos, BodyType type) {
    EMotionType motion;
    ObjectLayer layer;
    EActivation activation = EActivation::DontActivate; // Default

    switch (type) {
        case BodyType::Static:
            motion = EMotionType::Static;
            layer = Layers::NON_MOVING;
            break;
        case BodyType::Dynamic:
            motion = EMotionType::Dynamic;
            layer = Layers::MOVING;
            activation = EActivation::Activate; // Los cuerpos dinámicos suelen activarse
            break;
        case BodyType::Kinematic:
            motion = EMotionType::Kinematic;
            layer = Layers::MOVING; // Los cinemáticos interactúan como los dinámicos
            activation = EActivation::Activate; // Los cinemáticos siempre están "activos"
            break;
    }

    auto& iface = GetBodyInterface();

    BodyCreationSettings settings(
        new SphereShape(radius),
        pos,
        Quat::sIdentity(),
        motion,
        layer
    );
    BodyID id = iface.CreateAndAddBody(settings, activation);
    std::shared_ptr<Body> body = std::make_shared<Body>(id, type);
    m_Bodies.push_back(body);
    return body;
}

#ifdef JPH_DEBUG_RENDERER
    void Physics::DrawBodies(JPH::BodyManager::DrawSettings& settings, JPH::DebugRenderer *debugRenderer)
    {
       m_PhysicsSystem->DrawBodies(settings, debugRenderer);
    }
#endif  

} // namespace Engine
