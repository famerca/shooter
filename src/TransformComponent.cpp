#include "TransformComponent.hpp"

TransformComponent::TransformComponent(GameObject * _owner): Component(_owner), position(0.f), _scale(1.f), axis(1.f)
{
   angle = 0.f;
   changed = false;
   movement = nullptr;
   this->updateModel();
}

TransformComponent::~TransformComponent()
{

}

void TransformComponent::updateModel()
{
    model = glm::mat4(1.f);

    model = glm::translate(model, position);
    model = glm::rotate(model, angle, axis);
    model = glm::scale(model, _scale);

    changed  = false;
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

void Movement::move(glm::vec3 move_vector)
{
    parent->position = parent->position + move_vector;
}