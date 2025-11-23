#include <iostream>
#include <string>
#include <stdexcept>
#include <random>
#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLS/Window.hpp>
#include "GLS/Scene.hpp"
#include "GLS/GameObject.hpp"
#include "GLS/ModelComponent.hpp"
#include "GLS/TransformComponent.hpp"
#include "GLS/DirectionalLight.hpp"
#include "GLS/Renderer.hpp"
#include "GLS/Input.hpp"
#include "GLS/SkyBox.hpp"
#include "GLS/Physics.hpp"
#include "GLS/Listener.hpp"
#include "GLS/AudioManager.hpp"
#include "GLS/AudioPlayer.hpp"
#include "GLS/AudioListenerComponent.hpp"
#include "GLS/AudioSourceComponent.hpp"
#include "GLS/Path.hpp"

#include "Game.hpp"


using namespace Engine;

int main()
{
    try
    {
        
        Engine::Physics::Get().Init();
        Engine::AudioManager::Get().init();

        // Window dimensions
        constexpr GLint WIDTH = 1200;
        constexpr GLint HEIGHT = 720;


        auto main_window = Window::create(WIDTH, HEIGHT, "Cubo y piramide");

        if (main_window == nullptr)
        {
            return EXIT_FAILURE;
        }

        Game game(main_window);
        game.init();
        
        game.render();

        Engine::Physics::Get().Shutdown();
        Engine::AudioManager::Get().shutdown();

    }catch(const std::exception& e)
    {

    }
    
    return EXIT_SUCCESS;
}
