#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

class TransformComponent;

class Movement
{
private:
    TransformComponent *parent;
public:
    Movement(TransformComponent *p)
    {
        parent = p;
    }
    ~Movement();

    void move(glm::vec3 move_vector) ;
    void setVelocity(glm::vec3) {};
    void setAceleration(glm::vec3) {};
    void inpulse(glm::vec3) {};

    void update(GLfloat) {};
};

#endif // MOVEMENT_HPP