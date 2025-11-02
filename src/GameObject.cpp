#include "GameObject.hpp"

GameObject::GameObject(std::shared_ptr<TransformComponent> t): components(), visible(true)
{
    transform = t;
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

void GameObject::update(GLfloat dt)
{
    transform->update(dt);
    for (std::shared_ptr<Component> component : components)
    {
        component->update(dt);
    }
}

void GameObject::addComponent(std::shared_ptr<Component> component)
{
    components.push_back(component);
}