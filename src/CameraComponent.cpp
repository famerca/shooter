#include "CameraComponent.hpp"
#include "Scene.hpp"

CameraComponent::CameraComponent(Owner owner, std::shared_ptr<Scene> scene): Component(owner, Component::Type::Camera), scene(scene)
{
    position = glm::vec3(0.f, 0.f, 0.f);
    aspectRation = 1.f;
    fov = 45.f;
    nearPlane = 0.1f;
    farPlane = 100.f;
    isOrthographic = false;
}

std::shared_ptr<CameraComponent> CameraComponent::create(Owner owner, std::shared_ptr<Scene> scene)
{
    return std::make_shared<CameraComponent>(owner, scene);
}

std::shared_ptr<CameraComponent> CameraComponent::self()
{
    return shared_from_this();
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
    changed = true;
}

void CameraComponent::setAspectRation(float aspect)
{
    aspectRation = aspect;
    changed = true;
}

void CameraComponent::setFov(float fov)
{
    this->fov = fov;
    changed = true;
}

void CameraComponent::setNearPlane(float near)
{
    nearPlane = near;
    changed = true;
}

void CameraComponent::setFarPlane(float far)
{
    farPlane = far;
    changed = true;
}

void CameraComponent::setOrthographic(bool orthographic)
{
    isOrthographic = orthographic;
    changed = true;
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
    scene->setCamera(self());
}
