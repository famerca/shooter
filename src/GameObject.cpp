#include "GameObject.hpp"

GameObject::GameObject(): components(), visible(true)
{
    transform = std::make_shared<TransformComponent>(std::make_shared<GameObject>(this));
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