#pragma once

#include <functional>
#include <iostream>
#include <memory>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include "GameObject.hpp"

#include "Scene.hpp"

namespace Engine {

// Definición del tipo de callback para manejar el evento OnContactAdded


// El Wrapper Singleton para la interfaz ContactListener de Jolt
class Listener final : public JPH::ContactListener {
    public:

    using Key = uint64_t;
    using Callback = std::function<void()>;

    enum class Event {
        ContactAdded,      // Inicio de la colisión
        ContactRemoved     // Fin de la colisión
        // Se podrían añadir ContactPersisted y ValidateContact si se necesitan
    };
    // 1. Singleton Access Method
    static Listener& Get();

    void Add(Event event, JPH::BodyID id1, JPH::BodyID id2, Callback callback);

    void Add(Event event,
        std::shared_ptr<GameObject> object1,
        std::shared_ptr<GameObject> object2,
        Callback callback
    );

    void Add(std::shared_ptr<Scene> scene, Event event, unsigned id1, unsigned id2, Callback callback);


    // 2. Métodos virtuales de Jolt (implementación para ejecutar las lambdas)
    
    // Se llama antes de que la colisión ocurra, para decidir si se acepta.
    virtual JPH::ValidateResult OnContactValidate(
        const JPH::Body &inBody1, 
        const JPH::Body &inBody2, 
        JPH::RVec3Arg inBaseOffset, 
        const JPH::CollideShapeResult &inCollisionResult) override;

    // Se llama cuando dos cuerpos comienzan a tocarse (Inicio de contacto).
    virtual void OnContactAdded(
        const JPH::Body &inBody1, 
        const JPH::Body &inBody2, 
        const JPH::ContactManifold &inManifold, 
        JPH::ContactSettings &ioSettings) override;

    // Se llama mientras dos cuerpos continúan tocándose (Contacto persistente).
    virtual void OnContactPersisted(
        const JPH::Body &inBody1, 
        const JPH::Body &inBody2, 
        const JPH::ContactManifold &inManifold, 
        JPH::ContactSettings &ioSettings) override;

    // Se llama cuando dos cuerpos dejan de tocarse (Fin de contacto).
    virtual void OnContactRemoved(
        const JPH::SubShapeIDPair &inSubShapePair) override;

private:
    // Constructor privado para el patrón Singleton
    Listener() = default;

    // Prohibir la copia y la asignación
    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    static Key CreateKey(JPH::BodyID id1, JPH::BodyID id2);

    // Variables internas para almacenar las lambdas registradas
    std::unordered_map<Key, std::vector<Callback>> m_CallbacksAdded;
    std::unordered_map<Key, std::vector<Callback>> m_CallbacksRemoved;
    // Podrías añadir ContactCallback m_OnContactPersisted si lo necesitas
};

} // namespace Engine