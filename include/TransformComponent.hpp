#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    void setVelocity(glm::vec3);
    void setAceleration(glm::vec3);
    void inpulse(glm::vec3);

    void update(GLfloat);
};


class TransformComponent : Component
{
    
private:

    
    float angle;

    glm::vec3 _scale;
    glm::mat4 model;
    glm::vec3 axis;

    bool changed;

    Movement * movement;

    /* data */
public:
    glm::vec3 position;
    TransformComponent(GameObject *);
    ~TransformComponent();

    void translate(float x , float y, float z);
    void translate(glm::vec3);
    
    void rotate(float angle, glm::vec3);

    void scale(float x, float y, float z);
    void scale(glm::vec3);


    void createMovement(glm::vec3 velocity, glm::vec3 acceleration);

    void update(GLfloat);

    void updateModel();

    glm::mat4 getModelMatrix();

    
};