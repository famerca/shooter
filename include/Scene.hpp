#include "TransformComponent.hpp"
#include "ModelComponent.hpp"
#include "DirectionalLight.hpp"
#include "CameraComponent.hpp"
#include "Window.hpp"
#include "GameObject.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef SCENE_HPP
#define SCENE_HPP


class Scene
{
friend class Renderer;

private:
    std::vector<std::shared_ptr<GameObject>> Objects;
    std::shared_ptr<CameraComponent> activeCamera;
    std::shared_ptr<DirectionalLight> DirLight;
    std::shared_ptr<Window> window;
    std::shared_ptr<Scene> shared;
    
public:
    Scene(std::shared_ptr<Window> window);
    Scene(std::shared_ptr<Window> window, std::shared_ptr<DirectionalLight> DirLight);
    Scene(std::shared_ptr<Window> window, glm::vec3 direction, glm::vec3 color, GLfloat intensity);
    Scene(Scene *p){
        this->window = p->window;
        this->DirLight = p->DirLight;
        this->activeCamera = p->activeCamera;
        this->Objects = p->Objects;
        this->shared = p->shared;
    }
    ~Scene();

    std::shared_ptr<GameObject> operator[](unsigned index) noexcept;
    std::shared_ptr<GameObject> at(unsigned index) noexcept;

    unsigned createGameObject();
    std::shared_ptr<ModelComponent> createModel(unsigned);
    std::shared_ptr<CameraComponent> createCamera(unsigned);
    
    void setCamera(std::shared_ptr<CameraComponent> camera);


    void update(GLfloat dt);

    std::shared_ptr<Window> getWindow();

    
};

#endif // SCENE_HPP

