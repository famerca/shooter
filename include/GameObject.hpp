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
    GameObject();
    GameObject(GameObject *p)
    {
        transform = p->transform;
        components = p->components;
        visible = p->visible;
    }
    ~GameObject();

    bool isVisible();
    void setVisible(bool);

    void addComponent(std::shared_ptr<Component>);

    //temp
    std::vector<std::shared_ptr<Component>> getComponents() const
    {
        return components;
    }

    std::shared_ptr<TransformComponent> getTransform();
    void update(const GLfloat &);
};

#endif // GAMEOBJECT_HPP