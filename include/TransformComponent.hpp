#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Component.hpp"

#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

class TransformComponent : Component
{
    
private:

    
    glm::vec3 _scale;
    glm::mat4 model;
    glm::quat rotation;
    glm::vec3 position;

    /* data */
public:
    TransformComponent(Owner);
    ~TransformComponent();

    void translate(float x , float y, float z);
    void translate(glm::vec3);
    
    void rotate(float angle, glm::vec3);
    void rotate(const glm::quat& r);
    void rotate(float w, float x, float y, float z);

    void scale(float x, float y, float z);
    void scale(glm::vec3);

    void update();

    void updateModel();

    glm::vec3 getPosition();

    glm::mat4 getModelMatrix();

    
};

#endif // TRANSFORM_COMPONENT_HPP