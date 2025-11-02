#include "Component.hpp"
#include "TransformComponent.hpp"
#include <vector>

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

class GameObject
{
private:
    /* data */
    std::shared_ptr<TransformComponent> transform;

    std::vector<std::shared_ptr<Component>> components;
    bool visible;
public:
    GameObject(std::shared_ptr<TransformComponent>);
    ~GameObject();

    bool isVisible();
    void setVisible(bool);

    void addComponent(std::shared_ptr<Component>);

    std::shared_ptr<TransformComponent> getTransform();
    void update(GLfloat);
};

#endif // GAMEOBJECT_HPP