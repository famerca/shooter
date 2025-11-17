#include "Listener.hpp"
#include "Physics.hpp"
#include <Jolt/Physics/Body/Body.h>

namespace Engine {

// Implementación del método Singleton
Listener& Listener::Get() {
    static Listener instance;
    return instance;
}

// 1. Implementación de ValidateContact
// Generalmente, aquí devolvemos 'AcceptAllContacts' a menos que el usuario lo cambie.
JPH::ValidateResult Listener::OnContactValidate(
    const JPH::Body &inBody1, 
    const JPH::Body &inBody2, 
    JPH::RVec3Arg inBaseOffset, 
    const JPH::CollideShapeResult &inCollisionResult) 
{
    // Acepta todas las colisiones por defecto. 
    // Los filtros de colisión deberían gestionarse en la capa de BodyFilter o ObjectLayerFilter de Jolt.
    JPH::BodyInterface& bodyInterface = Physics::Get().GetBodyInterface();
    
    // Umbral de velocidad ascendente para ignorar la colisión
    const float JUMP_SEPARATION_VELOCITY_THRESHOLD = 1.0f; 

    const JPH::Body* bodies[2] = { &inBody1, &inBody2 };
    
    for (int i = 0; i < 2; ++i) {
        const JPH::Body* dynamicBody = bodies[i];
        
        // Solo verificamos cuerpos dinámicos, ya que los estáticos no se mueven por impulso.
        if (dynamicBody->IsDynamic()) {
            
            JPH::Vec3 linearVelocity = dynamicBody->GetLinearVelocity();
            //std::cout << linearVelocity << std::endl;
            float velocityY = linearVelocity.GetY();
            
            // Si la velocidad vertical es alta y positiva (indicando un salto o separación forzada)
            if (velocityY > JUMP_SEPARATION_VELOCITY_THRESHOLD) {
                
                // Ignorar el contacto. Jolt no llamará a OnContactAdded.
                return JPH::ValidateResult::RejectContact;
            }
        }
    }
    
    // Si no se cumple la condición de salto, permitimos que el contacto continúe normalmente.
    // Esto asegura que otras colisiones sí se procesen.
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

Listener::Key Listener::CreateKey(JPH::BodyID id1, JPH::BodyID id2) 
{
    // Garantiza que el orden de las IDs no importa.
    // Usamos std::min/max para que (A, B) y (B, A) siempre generen la misma clave.
    uint32_t lower = std::min(id1.GetIndex(), id2.GetIndex());
    uint32_t upper = std::max(id1.GetIndex(), id2.GetIndex());

    // Combina los dos uint32_t en un solo uint64_t.
    // Los IDs de Jolt son uint32_t, por lo que esto es seguro.
    return (static_cast<Listener::Key>(upper) << 32) | static_cast<Listener::Key>(lower);
}

void Listener::Add(Event event, JPH::BodyID id1, JPH::BodyID id2, Callback callback)
{
    // 1. Crear la clave única para el par de IDs
    Listener::Key key = CreateKey(id1, id2);
    
    // 2. Registrar la lambda en el mapa correspondiente
    switch (event) {
        case Listener::Event::ContactAdded:
            m_CallbacksAdded[key].push_back(std::move(callback));
            break;
        case  Listener::Event::ContactRemoved:
            m_CallbacksRemoved[key].push_back(std::move(callback));
            break;
        default:
            // Manejo de errores si el evento no es compatible
            break;
    }
}

void Listener::Add(Event event,
    std::shared_ptr<GameObject> object1,
    std::shared_ptr<GameObject> object2,
    Callback callback)
{

    JPH::BodyID id1 = object1->body->m_BodyID;
    JPH::BodyID id2 = object2->body->m_BodyID;

    Add(event, id1, id2, callback);

}

void Listener::Add(std::shared_ptr<Scene> sc, Event event, unsigned id1, unsigned id2, Callback callback)
{
    if(sc->at(id1)->body == nullptr)
        throw std::runtime_error("Scene::at(id1) no tiene un body asignado");
    if(sc->at(id2)->body == nullptr)
        throw std::runtime_error("Scene::at(id2) no tiene un body asignado");
    Add(event, sc->at(id1)->body->m_BodyID, sc->at(id2)->body->m_BodyID, callback);
}

// 2. Implementación de OnContactAdded (Inicio de Contacto)
void Listener::OnContactAdded(
    const JPH::Body &inBody1, 
    const JPH::Body &inBody2, 
    const JPH::ContactManifold &inManifold, 
    JPH::ContactSettings &ioSettings) 
{
    if(m_CallbacksAdded.empty())
        return;
    // 1. Calcular la clave de la colisión actual
    Key key = CreateKey(inBody1.GetID(), inBody2.GetID());

    // 2. Buscar la clave en el mapa de CallbacksAdded
    auto it = m_CallbacksAdded.find(key);

    if (it != m_CallbacksAdded.end()) {
        // 3. Si se encuentra, iterar y ejecutar todas las lambdas registradas para ese par
        for (const Callback& callback : it->second) {
            callback(); // Llama a la lambda sin parámetros
        }
    }
}

// 3. Implementación de OnContactPersisted (Contacto Continuo)
void Listener::OnContactPersisted(
    const JPH::Body &inBody1, 
    const JPH::Body &inBody2, 
    const JPH::ContactManifold &inManifold, 
    JPH::ContactSettings &ioSettings) 
{
    // Por defecto, no hacemos nada aquí. Si se necesita persistencia, el usuario 
    // puede modificar la clase para incluir un callback para este evento.
    
    // NOTA: Si quisieras una lambda para esto, la lógica sería similar a OnContactAdded.
}

// 4. Implementación de OnContactRemoved (Fin de Contacto)
void Listener::OnContactRemoved(
    const JPH::SubShapeIDPair &inSubShapePair) 
{
    // Nota: Jolt solo proporciona los SubShapeID en este callback, no los BodyID.
    // Esto se complica si quieres saber qué cuerpos se separaron.
    // Una forma común es obtener el BodyID del SubShapeID.
    // Para simplificar, asumiremos que si el usuario registra una lambda para Remove,
    // es para un caso donde el BodyID no es estrictamente necesario o se resuelve aparte.
    if(m_CallbacksRemoved.empty())
        return;

    // 1. Calcular la clave de la colisión actual
    Key key = CreateKey(inSubShapePair.GetBody1ID(), inSubShapePair.GetBody2ID());

    // 2. Buscar la clave en el mapa de CallbacksRemoved
    auto it = m_CallbacksRemoved.find(key);

    if (it != m_CallbacksRemoved.end()) {
        // 3. Si se encuentra, iterar y ejecutar todas las lambdas registradas para ese par
        for (const Callback& callback : it->second) {
            callback(); // Llama a la lambda sin parámetros
        }   
    }
}

} // namespace Engine