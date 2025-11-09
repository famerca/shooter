#include "TransformComponent.hpp"
#include "ModelComponent.hpp"
#include "DirectionalLight.hpp"
#include "CameraComponent.hpp"
#include "Window.hpp"
#include "GameObject.hpp"
#include "SkyBox.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef SCENE_HPP
#define SCENE_HPP


class Scene: public std::enable_shared_from_this<Scene>
{
friend class Renderer;

private:
    std::vector<std::shared_ptr<GameObject>> Objects;
    std::shared_ptr<CameraComponent> activeCamera;
    std::shared_ptr<DirectionalLight> DirLight;
    std::shared_ptr<Window> window;
     std::shared_ptr<SkyBox> skyBox;
    
public:
    Scene(std::shared_ptr<Window> window);
    Scene(std::shared_ptr<Window> window, std::shared_ptr<DirectionalLight> DirLight);
    Scene(std::shared_ptr<Window> window, glm::vec3 direction, glm::vec3 color, GLfloat intensity);
    static std::shared_ptr<Scene> create(std::shared_ptr<Window> window);
    static std::shared_ptr<Scene> create(std::shared_ptr<Window> window, std::shared_ptr<DirectionalLight> DirLight);
    static std::shared_ptr<Scene> create(std::shared_ptr<Window> window, glm::vec3 direction, glm::vec3 color, GLfloat intensity);
    
    std::shared_ptr<Scene> self();

    ~Scene();

    std::shared_ptr<GameObject> operator[](unsigned index) noexcept;
    std::shared_ptr<GameObject> at(unsigned index) noexcept;

    unsigned createGameObject();
    std::shared_ptr<ModelComponent> createModel(unsigned);
    std::shared_ptr<CameraComponent> createCamera(unsigned);
    
    void setCamera(std::shared_ptr<CameraComponent> camera);
    std::shared_ptr<CameraComponent> getCamera();


    void update(const GLfloat &dt);

    std::shared_ptr<Window> getWindow();

    void setSkyBox(std::shared_ptr<SkyBox> sky_box);
    std::shared_ptr<SkyBox> getSkyBox();

    
};

#endif // SCENE_HPP

