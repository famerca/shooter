#include "TransformComponent.hpp"
#include "GameObject.hpp"

TransformComponent::TransformComponent(Owner _owner): Component(_owner, Component::Type::Transform)
{
  
   position = glm::vec3(0.f, 0.f, 0.f);
   _scale = glm::vec3(1.f, 1.f, 1.f);
   rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
   this->updateModel();
   changed = false;
}

glm::vec3 TransformComponent::getPosition()
{
    return position;
}

glm::quat TransformComponent::getRotation()
{
    return rotation;
}

TransformComponent::~TransformComponent()
{

}

void TransformComponent::updateModel()
{
    model = glm::mat4(1.f);

    model = glm::translate(model, position);

    model *= glm::mat4_cast(rotation);

    model = glm::scale(model, _scale);

    changed  = false;
}

void TransformComponent::update()
{
    if(changed)
    {
        updateModel();
    }
}

const glm::mat4& TransformComponent::getModelMatrix()
{
    return model;
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
    owner->change();
}

void TransformComponent::rotate(float angle, glm::vec3 axis)
{
// 1. Convertir el ángulo de grados a radianes.
    float rad_angle = glm::radians(angle);

    // 2. Crear un nuevo cuaternión de rotación a partir del Eje-Ángulo.
    // axis debe ser un vector unitario, es buena práctica normalizarlo.
    glm::quat rotation_delta = glm::angleAxis(rad_angle, glm::normalize(axis));

    // 3. Aplicar la nueva rotación al cuaternión existente.
    // Multiplicar cuaterniones equivale a concatenar rotaciones. 
    // La convención común es: nueva_rotación * rotación_actual (post-multiplicación)
    rotation = rotation_delta * rotation; 

    // 4. Marcar como cambiado para que se reconstruya la matriz
    changed = true;
    owner->change();
}

void TransformComponent::rotate(const glm::quat& r)
{
    rotation = r;
    changed = true;
    owner->change();
}

void TransformComponent::rotate(float w, float x, float y, float z)
{
    glm::quat r(w, x, y, z);
    rotate(r);
}

void TransformComponent::scale(float x, float y, float z)
{
    _scale = glm::vec3(x, y, z);
    changed = true;
    owner->change();
}

void TransformComponent::scale(glm::vec3 scale)
{
    _scale = scale;
    changed = true;
}


