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

int main()
{
    try
    {
        // Window dimensions
        constexpr GLint WIDTH = 800;
        constexpr GLint HEIGHT = 600;

        auto main_window = Window::create(WIDTH, HEIGHT, "Cubo y piramide");

        if (main_window == nullptr)
        {
            return EXIT_FAILURE;
        }
        auto input = Input::create();
        main_window->setInput(input);

        auto light = std::make_shared<DirectionalLight>(1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.5f);
        auto scene = Scene::create(main_window, light);

        auto cameraObject = scene->createGameObject();
        auto camera = scene->createCamera(cameraObject);

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
