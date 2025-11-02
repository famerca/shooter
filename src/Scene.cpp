#include "Scene.hpp"

Scene::Scene(std::shared_ptr<Window> window)
{
    this->window = window;
    activeCamera = nullptr;
    DirLight = nullptr;
}

Scene::Scene(std::shared_ptr<Window> window, shared_ptr<DirectionalLight> DirLight)
{
    this->window = window;
    this->DirLight = DirLight;
    activeCamera = nullptr;
}

Scene::Scene(std::shared_ptr<Window> window, glm::vec3 direction, glm::vec3 color, GLfloat intensity)
{
    this->window = window;
    DirLight = std::make_shared<DirectionalLight>(direction, color, intensity);
    activeCamera = nullptr;
}

Scene::~Scene()
{
}

void Scene::setCamera(std::shared_ptr<CameraComponent> camera)
{
    activeCamera = camera;
}

void Scene::update(GLfloat dt)
{
    for (std::shared_ptr<GameObject> object : Objects)
    {
        object->update(dt);
    }
}

std::shared_ptr<Window> Scene::getWindow()
{
    return window;
}