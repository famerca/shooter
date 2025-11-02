#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef CAMERA_COMPONENT_HPP
#define CAMERA_COMPONENT_HPP

class Scene;

class CameraComponent : public Component
{

private:
    glm::vec3 position;
    float aspectRation;
    float fov;
    float nearPlane;
    float farPlane;
    bool isOrthographic;
    glm::mat4 view;
    glm::mat4 projection;
    std::shared_ptr<Scene> scene;

public:
    CameraComponent(Owner, std::shared_ptr<Scene> scene);
    CameraComponent(CameraComponent *p) : Component(p->owner, Component::Type::Camera)
    {
        position = p->position;
        aspectRation = p->aspectRation;
        fov = p->fov;
        nearPlane = p->nearPlane;
        farPlane = p->farPlane;
        isOrthographic = p->isOrthographic;
        view = p->view;
        projection = p->projection;
        scene = p->scene;
    }
    ~CameraComponent();

    void update(GLfloat) {};

    void init(glm::vec3 pos, float aspect, float fov, float near, float far, bool orthographic);

    void setPosition(glm::vec3);
    void setAspectRation(float);
    void setFov(float);
    void setNearPlane(float);
    void setFarPlane(float);
    void setOrthographic(bool);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    void activate();
};

#endif // CAMERA_COMPONENT_HPP