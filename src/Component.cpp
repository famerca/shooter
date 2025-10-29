#include "Component.hpp"

Component::Component(GameObject *_owner)
{
    owner = _owner;
}

Component::~Component()
{
    owner = nullptr;
}

GameObject* Component::getOwner()
{
    return owner;
}