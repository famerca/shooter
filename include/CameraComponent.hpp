#include "Component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef CAMERA_COMPONENT_HPP
#define CAMERA_COMPONENT_HPP

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
    bool rotation_changed{false};

    GLfloat pitch;
    GLfloat yaw;

    void updateRotation(const GLfloat &dt);
    
public:
    CameraComponent(Owner, std::shared_ptr<Scene> scene);
    static std::shared_ptr<CameraComponent> create(Owner owner, std::shared_ptr<Scene> scene);
    
    std::shared_ptr<CameraComponent> self();

    ~CameraComponent();

    void update(const GLfloat &);

    void init(glm::vec3 pos, float aspect, float fov, float near, float far, bool orthographic);

    void setPosition(glm::vec3);
    void setAspectRation(float);
    void setFov(float);
    void setNearPlane(float);
    void setFarPlane(float);
    void setOrthographic(bool);
    void setFront(glm::vec3);
    void setFront(GLfloat pitch, GLfloat yaw);
    void setUp(glm::vec3);

    void setRotation(GLfloat x, GLfloat y);
    void calcFront();
    

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    void activate();
};

#endif // CAMERA_COMPONENT_HPP