#include "Scene.hpp"

Scene::Scene(std::shared_ptr<Window> window)
{
    this->window = window;
    activeCamera = nullptr;
    DirLight = nullptr;
}

Scene::Scene(std::shared_ptr<Window> window, std::shared_ptr<DirectionalLight> DirLight)
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


std::shared_ptr<Scene> Scene::create(std::shared_ptr<Window> window)
{
    return std::make_shared<Scene>(window);
}

std::shared_ptr<Scene> Scene::create(std::shared_ptr<Window> window, std::shared_ptr<DirectionalLight> DirLight)
{
    return std::make_shared<Scene>(window, DirLight);
}

std::shared_ptr<Scene> Scene::create(std::shared_ptr<Window> window, glm::vec3 direction, glm::vec3 color, GLfloat intensity)
{
    return std::make_shared<Scene>(window, direction, color, intensity);
}

std::shared_ptr<Scene> Scene::self()
{
    return shared_from_this();
}

Scene::~Scene()
{
}

void Scene::setCamera(std::shared_ptr<CameraComponent> camera)
{
    activeCamera = camera;
}

unsigned Scene::createGameObject()
{
    auto object = std::make_shared<GameObject>();
    Objects.push_back(object);
    return Objects.size() - 1;
}

std::shared_ptr<ModelComponent> Scene::createModel(unsigned index)
{
    if(index >= Objects.size())
    {
        throw std::runtime_error("Index out of range");
    }
    auto model = std::make_shared<ModelComponent>(Objects[index]);
    Objects[index]->addComponent(std::static_pointer_cast<Component>(model));
    return model;
}

std::shared_ptr<CameraComponent> Scene::createCamera(unsigned index)
{
    if(index >= Objects.size())
    {
        throw std::runtime_error("Index out of range");
    }
    auto camera = CameraComponent::create(Objects[index], this->self());
    Objects[index]->addComponent(std::static_pointer_cast<Component>(camera));
    return camera;
}

std::shared_ptr<GameObject> Scene::at(unsigned index) noexcept
{
    // Comprobación de límites: el índice debe ser menor que el tamaño del vector.
    if (index < Objects.size()) 
    {
        // Si es válido, devuelve el shared_ptr del GameObject.
        return Objects[index];
    }
    
    // Si está fuera de rango, devuelve nullptr, como solicitaste.
    return nullptr;
}
std::shared_ptr<GameObject> Scene::operator[](unsigned index) noexcept
{
    return at(index);
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