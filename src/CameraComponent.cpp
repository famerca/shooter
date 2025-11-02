#include "CameraComponent.hpp"

CameraComponent::CameraComponent(GameObject *owner): Component(owner)
{
    position = glm::vec3(0.f, 0.f, 0.f);
    aspectRation = 1.f;
    fov = 45.f;
    nearPlane = 0.1f;
    farPlane = 100.f;
    isOrthographic = false;
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::init(glm::vec3 pos, float aspect, float fov, float near, float far, bool orthographic)
{
    position = pos;
    aspectRation = aspect;
    this->fov = fov;
    nearPlane = near;
    farPlane = far;
    isOrthographic = orthographic;

    view = glm::lookAt(position, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    projection = glm::perspective(glm::radians(fov), aspectRation, nearPlane, farPlane);
}

void CameraComponent::setPosition(glm::vec3 pos)
{
    position = pos;
}

void CameraComponent::setAspectRation(float aspect)
{
    aspectRation = aspect;
}

void CameraComponent::setFov(float fov)
{
    this->fov = fov;
}

void CameraComponent::setNearPlane(float near)
{
    nearPlane = near;
}

void CameraComponent::setFarPlane(float far)
{
    farPlane = far;
}

void CameraComponent::setOrthographic(bool orthographic)
{
    isOrthographic = orthographic;
}

glm::mat4 CameraComponent::getViewMatrix()
{
    return view;
}

glm::mat4 CameraComponent::getProjectionMatrix()
{
    return projection;
}

void CameraComponent::activate()
{
    scene->setCamera(std::make_shared<CameraComponent>(this));
}
