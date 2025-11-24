#include <GL/glew.h>
#include <memory>

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

namespace Engine
{

class GameObject;
using Owner = std::shared_ptr<GameObject>;

class Component
{

public:
    enum class Type {
        Model, // Valor por defecto
        Transform,
        Camera,
        AudioListener,
        AudioSource,
        // ... más tipos
    };

    Component(Owner, Type);
    Component(const Component& other);
    virtual ~Component();

    virtual void update(const GLfloat &) {};
    virtual void start() {};
    Owner getOwner();
    Type getType() const noexcept;
    bool isChanged();

    void change();
    bool isRenderd();


protected:
    /* data */
    Owner owner;
    bool changed;
    bool renderd;

private:
    const Type type;
};

}
#endif