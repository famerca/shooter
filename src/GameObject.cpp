#include "GameObject.hpp"

GameObject::GameObject(Component *t): components(), visible(true)
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

Component * GameObject::getTransform()
{
    return transform;
}