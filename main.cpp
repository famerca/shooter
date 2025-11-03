#include <iostream>
#include <string>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Window.hpp>
#include "Scene.hpp"
#include "GameObject.hpp"
#include "ModelComponent.hpp"
#include "TransformComponent.hpp"
#include "DirectionalLight.hpp"
#include "Renderer.hpp"
#include "Input.hpp"

class inputManager: public Input
{
    std::shared_ptr<GameObject> object{nullptr};
    std::shared_ptr<Scene> scene{nullptr};
    float sensitivity{1.f};
    float speed{1.f};
public:
    inputManager() : Input() {}

    void init(std::shared_ptr<Scene> scene, std::shared_ptr<GameObject> object)
    {
        this->object = object;
        this->scene = scene;
    }

    void update(const float &dt) noexcept {
        glm::vec3 pos = object->getTransform()->getPosition();

        if(is_key_pressed(GLFW_KEY_W))
        {
            object->getTransform()->translate(pos + glm::vec3(0.f, 0.f, 1.f) * speed * dt);
        }

        if(is_key_pressed(GLFW_KEY_S))
        {
            object->getTransform()->translate(pos + glm::vec3(0.f, 0.f, -1.f) * speed * dt);
        }  

        if(is_key_pressed(GLFW_KEY_A))
        {
            object->getTransform()->translate(pos + glm::vec3(1.f, 0.f, 0.f) * speed * dt);
        }

        if(is_key_pressed(GLFW_KEY_D))
        {
            object->getTransform()->translate(pos + glm::vec3(-1.f, 0.f, 0.f) * speed * dt);
        }

        std::cout << "delta: " << dt <<  ", FPS: " << 1.f / dt << std::endl;

        MousePair delta = this->get_mouse_delta();
        if(delta.x != 0.f || delta.y != 0.f)
        {
           // std::cout << "x: " << delta.x << ", y: " << delta.y << std::endl;
            auto camera = scene->getCamera();
            if(camera != nullptr)
            {
                camera->setRotation(-delta.x * sensitivity, -delta.y * sensitivity);
            }
        }

    }

    ~inputManager() {}
};

int main()
{
    try
    {
        // Window dimensions
        constexpr GLint WIDTH = 1200;
        constexpr GLint HEIGHT = 720;

        auto main_window = Window::create(WIDTH, HEIGHT, "Cubo y piramide");

        if (main_window == nullptr)
        {
            return EXIT_FAILURE;
        }
        auto input = std::make_shared<inputManager>();
        main_window->setInput(input);

        auto light = std::make_shared<DirectionalLight>(1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.5f);
        auto scene = Scene::create(main_window, light);

        auto cameraObject = scene->createGameObject();
        auto camera = scene->createCamera(cameraObject);

        scene->at(cameraObject)->getTransform()->translate(glm::vec3(0.f, 0.f, -5.f));
        
        camera->setFront(0.f, 90.f);
        camera->setUp(glm::vec3(0.f, 1.f, 0.f));

        input->init(scene, scene->at(cameraObject));

        camera->init(glm::vec3(0.f, 0.f, 0.f), main_window->get_aspect_ratio(), 45.f, 0.1f, 100.f, true);
        camera->activate();

        auto suzanne = scene->createGameObject();
        scene->createModel(suzanne)->loadModel("suzanne.fbx");

        auto sphere = scene->createGameObject();
        scene->createModel(sphere)->loadModel("sphere.fbx");

        scene->at(sphere)->getTransform()->translate(1.f, 0.f, -2.5f);
        scene->at(sphere)->getTransform()->scale(0.4f, 0.4f, 0.4f);
        scene->at(suzanne)->getTransform()->translate(-1.f, 0.f, -2.5f);
        scene->at(suzanne)->getTransform()->scale(0.4f, 0.4f, 0.4f);

        auto renderer = std::make_shared<Renderer>();
        renderer->init();
        renderer->render(scene);

    }catch(const std::exception& e)
    {

    }
    
    return EXIT_SUCCESS;
}
