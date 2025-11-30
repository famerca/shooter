#ifndef SCRIPT_HPP
#define SCRIPT_HPP
#include "Component.hpp"

namespace Engine
{

class Body;

class ScriptComponent : public Component
{

private:


public:
    ScriptComponent();
    virtual ~ScriptComponent() = default;

    // Inyectamos el padre al crearlo o adjuntarlo
    void setGameObject(Owner go);

    void update(const GLfloat &dt) final override;
    void start() final override;  

protected:

    std::shared_ptr<Body> body;

    // Eventos opcionales
    virtual void OnStart();
    virtual void OnUpdate(const GLfloat &dt);
    virtual void OnPhysicsUpdate(float dt); // Para fuerzas de Jolt
    //virtual void OnCollisionEnter(GameObject* other) {}
};

}
#endif // SCRIPT_HPP