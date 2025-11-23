#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef CAMERA_COMPONENT_HPP
#define CAMERA_COMPONENT_HPP

namespace Engine
{

class Scene;

class CameraComponent : public Component, public std::enable_shared_from_this<CameraComponent>
{

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float aspectRation;
    float fov;
    float nearPlane;
    float farPlane;
    bool isOrthographic;
    glm::mat4 view;
    glm::mat4 projection;
    std::shared_ptr<Scene> scene;

    float x_rotation{0.f};
    float y_rotation{0.f};

    float clampPitch = 89.0f;
    float clampYaw = 0.0f;
    bool rotation_changed{false};
    bool perpective_changed{false};

    GLfloat pitch;
    GLfloat yaw;

    float distance = 5.0f;      // Distancia al personaje
    glm::vec3 targetOffset = glm::vec3(0.0f, 1.5f, 0.0f); // Para mirar a la cabeza, no a los pies
    bool isOrbiting = true;  // Flag para cambiar de modo
  
    void updateRotation(const GLfloat &dt);
    void updatePerpective();
    
public:
    CameraComponent(Owner, std::shared_ptr<Scene> scene);
    static std::shared_ptr<CameraComponent> create(Owner owner, std::shared_ptr<Scene> scene);
    
    std::shared_ptr<CameraComponent> self();

    ~CameraComponent();

    void update(const GLfloat &);

    void init(glm::vec3 pos, float aspect, float fov, float near, float far, bool orthographic);

    void setDistance(float dist);

    void setPosition(glm::vec3);
    void setAspectRation(float);
    void setFov(float);
    void setNearPlane(float);
    void setFarPlane(float);
    void setOrthographic(bool);
    void setFront(glm::vec3);
    void setFront(GLfloat pitch, GLfloat yaw);
    void setUp(glm::vec3);

    void rotate(GLfloat x, GLfloat y);

    void setOrbiting(bool orbiting);
    void setClamp(float pitch, float yaw);

    void calcFront();
    

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    void activate();
};

}

#endif // CAMERA_COMPONENT_HPP