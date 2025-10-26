#include <GL/glew.h>

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class GameObject;

class Component
{
private:
    /* data */
    
    GameObject *owner;

public:
    Component(GameObject *);
    ~Component();

    virtual void update(GLfloat);
    virtual void start();
    GameObject* getOwner();
};

#endif