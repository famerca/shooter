#include "Component.hpp"
namespace Engine
{

Component::Component(Owner _owner, Type _type): type(_type)
{
    owner = _owner;
    changed = true;
}

Component::~Component()
{
    owner = nullptr;
}

Owner Component::getOwner()
{
    return owner;
}

bool Component::isChanged()
{
    return changed;
}

Component::Type Component::getType() const noexcept
{
    return type;
}

void Component::change()
{
    changed = true;
}

bool Component::isRenderd()
{
    return renderd;
}

}