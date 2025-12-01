#include "GameObject.hpp"
#include "Physics.hpp"
#include <iostream>

namespace Engine
{

GameObject::GameObject(): components(), visible(true), transform{nullptr}, body{nullptr}
{
}

std::shared_ptr<GameObject> GameObject::create()
{
    auto go = std::make_shared<GameObject>();

    go->transform = std::make_shared<TransformComponent>(go);

    return go;

}

std::shared_ptr<GameObject> GameObject::clone()
{
    auto go = std::make_shared<GameObject>();

    go->transform = std::make_shared<TransformComponent>(go, *this->transform);
   
    if(this->body != nullptr)
    {
        go->body = Physics::Get().CloneBody(this->body);
        go->body->SetOwner(go);
    }

    if(this->movement != nullptr)
        go->movement = std::make_shared<KMovement>(go, *this->movement);

    go->components = this->components;
    go->visible = this->visible;

    return go;

}

std::shared_ptr<GameObject> GameObject::self()
{
    return shared_from_this();
}

GameObject::~GameObject()
{
}

bool GameObject::isVisible()
{
    return visible;
}

void GameObject::setVisible(bool v)
{
    visible = v;
}

std::shared_ptr<TransformComponent> GameObject::getTransform()
{
    return transform;
}

void GameObject::setBody(std::shared_ptr<Engine::Body> body)
{
    this->body = body;
    body->SetOwner(shared_from_this());
    body->SetPosition(transform->getPosition());
    body->SetRotation(transform->getRotation());

    if(body->getType() == Engine::BodyType::Kinematic)
    {
        movement = std::make_shared<Engine::KMovement>(shared_from_this());
    }
}

std::shared_ptr<Engine::Body> GameObject::getBody()
{
    return body;
}

std::shared_ptr<Engine::KMovement> GameObject::getMovement()
{
    return movement;
}

void GameObject::update(const GLfloat &dt)
{
    if(body != nullptr)
    {
        body->update();

        if(body->getType() == Engine::BodyType::Kinematic)
            movement->Update(dt);
    }

    transform->update();

    for (std::shared_ptr<Component> component : components)
    {
        component->update(dt);
    }
}

void GameObject::start()
{
    for (std::shared_ptr<Component> component : components)
    {
        component->start();
    }
}

void GameObject::addComponent(std::shared_ptr<Component> component)
{
    components.push_back(component);
}

void GameObject::change()
{
    for (std::shared_ptr<Component> component : components)
    {
        component->change();
    }
}

void GameObject::addScript(std::shared_ptr<ScriptComponent> script)
{
    script->setGameObject(shared_from_this());
    components.push_back(script);
}

} // namespace Engine