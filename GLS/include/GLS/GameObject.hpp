#include "Component.hpp"
#include "TransformComponent.hpp"
#include "Body.hpp"
#include "KMovement.hpp"
#include <vector>

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

namespace Engine {

class CameraComponent;

class Listener;

class GameObject: public std::enable_shared_from_this<GameObject>
{
private:
    /* data */
    friend class Engine::Listener;
    
    std::shared_ptr<TransformComponent> transform;
    std::shared_ptr<Engine::Body> body;
    std::shared_ptr<Engine::KMovement> movement;

    std::vector<std::shared_ptr<Component>> components;
    bool visible;

public:

    static std::shared_ptr<GameObject> create();
    GameObject();
    ~GameObject();
    std::shared_ptr<GameObject> self();

    std::shared_ptr<Engine::GameObject> clone();

    bool isVisible();
    void setVisible(bool);

    void setBody(std::shared_ptr<Engine::Body> body);
    std::shared_ptr<Engine::Body> getBody();
    std::shared_ptr<Engine::KMovement> getMovement();

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

}



#endif // GAMEOBJECT_HPP