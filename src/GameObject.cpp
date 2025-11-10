#include "GameObject.hpp"

GameObject::GameObject(): components(), visible(true), transform{nullptr}, body{nullptr}
{
}

std::shared_ptr<GameObject> GameObject::create()
{
    auto go = std::make_shared<GameObject>();

    go->transform = std::make_shared<TransformComponent>(go);

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
}

std::shared_ptr<Engine::Body> GameObject::getBody()
{
    return body;
}

void GameObject::update(const GLfloat &dt)
{
    if(body != nullptr)
        body->update();

    transform->update();

    for (std::shared_ptr<Component> component : components)
    {
        component->update(dt);
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