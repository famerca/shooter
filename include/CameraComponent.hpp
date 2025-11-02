#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef CAMERA_COMPONENT_HPP
#define CAMERA_COMPONENT_HPP

class Scene {
    public:
    void setCamera(std::shared_ptr<CameraComponent> camera);
};

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
    CameraComponent(GameObject *, std::shared_ptr<Scene> scene);
    ~CameraComponent();

    void update(GLfloat) {};

    void init(glm::vec3, float, float, float, float, bool);

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