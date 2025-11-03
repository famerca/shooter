#include "Component.hpp"
#include "TransformComponent.hpp"
#include <vector>

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

class GameObject: public std::enable_shared_from_this<GameObject>
{
private:
    /* data */
    std::shared_ptr<TransformComponent> transform;

    std::vector<std::shared_ptr<Component>> components;
    bool visible;

public:

    static std::shared_ptr<GameObject> create();
    GameObject();
    ~GameObject();
    std::shared_ptr<GameObject> self();

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

    void change();
};

#endif // GAMEOBJECT_HPP