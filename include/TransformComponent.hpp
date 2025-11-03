#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Component.hpp"
#include "Movement.hpp"

#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

class TransformComponent : Component
{
    
private:

    
    float angle;

    glm::vec3 _scale;
    glm::mat4 model;
    glm::vec3 axis;

    Movement * movement;

    /* data */
public:
    glm::vec3 position;
    TransformComponent(Owner);
    ~TransformComponent();

    void translate(float x , float y, float z);
    void translate(glm::vec3);
    
    void rotate(float angle, glm::vec3);

    void scale(float x, float y, float z);
    void scale(glm::vec3);


    void createMovement(glm::vec3 velocity, glm::vec3 acceleration);

    void update(const GLfloat &dt);

    void updateModel();

    glm::mat4 getModelMatrix();

    
};

#endif // TRANSFORM_COMPONENT_HPP