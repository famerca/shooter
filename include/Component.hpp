#include <GL/glew.h>
#include <memory>

#ifndef COMPONENT_HPP
#define COMPONENT_HPP


class GameObject;
using Owner = std::shared_ptr<GameObject>;

class Component
{

public:
    enum class Type {
        Model, // Valor por defecto
        Transform,
        Camera,
        // ... más tipos
    };

    Component(Owner, Type);
    virtual ~Component();

    virtual void update(const GLfloat &) {};
    virtual void start() {};
    Owner getOwner();
    Type getType() const noexcept;
    bool isChanged();


protected:
    /* data */
    Owner owner;
    bool changed;

private:
    const Type type;
};

#endif