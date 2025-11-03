#include "TransformComponent.hpp"

TransformComponent::TransformComponent(Owner _owner): Component(_owner, Component::Type::Transform)
{
   angle = 0.f;
   position = glm::vec3(0.f, 0.f, 0.f);
   _scale = glm::vec3(1.f, 1.f, 1.f);
   axis = glm::vec3(0.f, 1.f, 0.f);
   movement = nullptr;
   this->updateModel();
   changed = false;
}

TransformComponent::~TransformComponent()
{

}

void TransformComponent::updateModel()
{
    model = glm::mat4(1.f);

    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(angle), axis);
    model = glm::scale(model, _scale);

    changed  = false;
}

void TransformComponent::update(const GLfloat &dt)
{
    if(changed)
    {
        updateModel();
    }
}

glm::mat4 TransformComponent::getModelMatrix()
{
    return model;
}


void TransformComponent::createMovement(glm::vec3 velocity = glm::vec3(0.f), glm::vec3 acceleration = glm::vec3(0.f))
{
    if(movement == nullptr)
    {
        movement = new Movement(this);
        movement->setVelocity(velocity);
        movement->setAceleration(acceleration);
    }
}

void TransformComponent::translate(float x , float y, float z)
{
    glm::vec3 translation(x, y, z);
    translate(translation);
}

void TransformComponent::translate(glm::vec3 translation)
{
    position = translation;
    changed = true;
}

void TransformComponent::rotate(float angle, glm::vec3 axis)
{
    this->angle = angle;
    this->axis = axis;
    changed = true;
}

void TransformComponent::scale(float x, float y, float z)
{
    _scale = glm::vec3(x, y, z);
    changed = true;
}

void TransformComponent::scale(glm::vec3 scale)
{
    _scale = scale;
    changed = true;
}

void Movement::move(glm::vec3 move_vector)
{
    parent->position = parent->position + move_vector;
}

