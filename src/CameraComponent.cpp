#include "CameraComponent.hpp"
#include "Scene.hpp"

CameraComponent::CameraComponent(Owner owner, std::shared_ptr<Scene> scene): Component(owner, Component::Type::Camera), scene(scene)
{
    position = glm::vec3(0.f, 0.f, 0.f);
    front = glm::vec3(0.f, 0.f, 1.f);
    up = glm::vec3(0.f, 1.f, 0.f);
    aspectRation = 1.f;
    fov = 45.f;
    nearPlane = 0.1f;
    farPlane = 100.f;
    isOrthographic = false;
    x_rotation = 0.f;
    y_rotation = 0.f;
    changed = true;
    renderd = false;
    pitch = 0.f;
    yaw = 0.f;
}

std::shared_ptr<CameraComponent> CameraComponent::create(Owner owner, std::shared_ptr<Scene> scene)
{
    return std::make_shared<CameraComponent>(owner, scene);
}

std::shared_ptr<CameraComponent> CameraComponent::self()
{
    return shared_from_this();
}

void CameraComponent::updateRotation(const GLfloat &dt)
{
    if(rotation_changed)
    {
        pitch += y_rotation * dt;
        yaw += x_rotation * dt;

        pitch = std::max(-89.f, std::min(89.f, pitch));

        calcFront();

        auto absPos = owner->getTransform()->getPosition() + position;
        view = glm::lookAt(absPos, absPos + front, up);

        x_rotation = 0.f;
        y_rotation = 0.f;

        rotation_changed = false;
        renderd = false;
    }
}

void CameraComponent::calcFront()
{
    front.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
    front = glm::normalize(front);
}

void CameraComponent::update(const GLfloat &dt)
{
    updateRotation(dt);

    if(changed)
    {
        auto absPos = owner->getTransform()->getPosition() + position;
        view = glm::lookAt(absPos, absPos + front, up);
        projection = glm::perspective(glm::radians(fov), aspectRation, nearPlane, farPlane);
        changed = false;
        renderd = false;
    }
}

void CameraComponent::setRotation(GLfloat x, GLfloat y)
{
    x_rotation = x;
    y_rotation = y;
    rotation_changed = true;
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

    update(1.f);
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

void CameraComponent::setFront(glm::vec3 front)
{
    this->front = front;
    changed = true;
}

void CameraComponent::setFront(GLfloat pitch, GLfloat yaw)
{
    this->pitch = pitch;
    this->yaw = yaw;
    calcFront();
    changed = true;
}

void CameraComponent::setUp(glm::vec3 up)
{
    this->up = up;
    changed = true;
}

glm::mat4 CameraComponent::getViewMatrix()
{
    renderd = true;
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
