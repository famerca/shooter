#include "Scene.hpp"
#include "AudioManager.hpp"

namespace Engine
{

Scene::Scene(std::shared_ptr<Window> window)
{
    this->window = window;
    activeCamera = nullptr;
    DirLight = nullptr;
    skyBox = nullptr;
}

Scene::Scene(std::shared_ptr<Window> window, std::shared_ptr<DirectionalLight> DirLight)
{
    this->window = window;
    this->DirLight = DirLight;
    activeCamera = nullptr;
    skyBox = nullptr;
}

Scene::Scene(std::shared_ptr<Window> window, glm::vec3 direction, glm::vec3 color, GLfloat intensity)
{
    this->window = window;
    DirLight = std::make_shared<DirectionalLight>(direction, color, intensity);
    activeCamera = nullptr;
    skyBox = nullptr;
}

std::shared_ptr<CameraComponent> Scene::getCamera()
{
    return activeCamera;
}

std::shared_ptr<Scene> Scene::create(std::shared_ptr<Window> window)
{
    return std::make_shared<Scene>(window);
}

std::shared_ptr<Scene> Scene::create(std::shared_ptr<Window> window, std::shared_ptr<DirectionalLight> DirLight)
{
    auto scene = std::make_shared<Scene>(window, DirLight);
    
    return scene;
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
    auto object = GameObject::create();
    Objects.push_back(object);
    return Objects.size() - 1;
}

unsigned Scene::cloneGameObject(unsigned index)
{
    Objects.push_back(Objects[index]->clone());
    return Objects.size() - 1;
}

std::shared_ptr<GameObject> Scene::cloneGameObject(std::shared_ptr<GameObject> object)
{
    auto clone = object->clone();
    Objects.push_back(clone);
    return clone;
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


void Scene::update(const GLfloat &dt)
{
    for (std::shared_ptr<GameObject> object : Objects)
    {
        object->update(dt);
    }
}

void Scene::start()
{
    for (std::shared_ptr<GameObject> object : Objects)
    {
        object->start();
    }
}

std::shared_ptr<Engine::AudioListenerComponent> Scene::createAudioListener(unsigned index)
{
    if(index >= Objects.size())
    {
        throw std::runtime_error("Index out of range");
    }
    auto audioListener = std::make_shared<Engine::AudioListenerComponent>(Objects[index]);
    Objects[index]->addComponent(std::static_pointer_cast<Component>(audioListener));
    return audioListener;
}

std::shared_ptr<Engine::AudioSourceComponent> Scene::createAudioSource(unsigned index, const std::string& path, bool loop, float volume, float minDesc, float maxDist)
{
    if(index >= Objects.size())
    {
        throw std::runtime_error("Index out of range");
    }
    auto audioSource = std::make_shared<Engine::AudioSourceComponent>(Objects[index], false, loop, volume, minDesc, maxDist);
    Objects[index]->addComponent(std::static_pointer_cast<Component>(audioSource));
    audioSource->setAudio(Engine::AudioManager::Get().addAudio(path));

    return audioSource;
}

std::shared_ptr<Window> Scene::getWindow()
{
    return window;
}

void Scene::setSkyBox(std::shared_ptr<SkyBox> sky_box)
{
    this->skyBox = sky_box;
}

std::shared_ptr<SkyBox> Scene::getSkyBox()
{
    return skyBox;
}

void Scene::onReseze(int width, int height)
{
    activeCamera->setAspectRation(width / height);
}

}