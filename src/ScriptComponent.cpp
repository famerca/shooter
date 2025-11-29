#include "ScriptComponent.hpp"
#include "GameObject.hpp"

namespace Engine
{
    ScriptComponent::ScriptComponent() : Component(nullptr, Component::Type::Script), body(nullptr)
    {
    }

    void ScriptComponent::setGameObject(Owner go)
    {
        owner = go;
    }

    void ScriptComponent::update(const GLfloat &dt)
    {
        if(owner != nullptr)
        {
            OnUpdate(dt);

            if(body != nullptr)
            {
                OnPhysicsUpdate(dt);
            }
        }
    }

    void ScriptComponent::start()
    {
        body = owner->getBody();

        if(owner != nullptr)
        {
            this->OnStart();
        }
    }

    void ScriptComponent::OnStart() {}

    void ScriptComponent::OnUpdate(const GLfloat &dt) {}

    void ScriptComponent::OnPhysicsUpdate(float dt) {}


}