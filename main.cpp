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
#include "GLS/Physics.hpp"
#include "GLS/AudioManager.hpp"
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


        auto main_window = Window::create(WIDTH, HEIGHT, "Game");

        if (main_window == nullptr)
        {
            return EXIT_FAILURE;
        }


        std::cout << "ROOT: " << ROOT_PATH << std::endl;
        std::cout << "Carpeta de recursos: " << ASSETS_PATH << std::endl;
        std::cout << "Carpeta de shaders: " << SHADERS_PATH << std::endl;

        Game game(main_window);
        game.init();

        game.Level1();
        
        game.render();

        // IMPORTANTE: Hacer shutdown explícito de UI ANTES de que Game se destruya
        // Esto asegura que el shutdown ocurra mientras el contexto OpenGL todavía existe
        // La librería GLS ya tiene las correcciones en UIManager::Shutdown() que desregistran
        // eventos antes de cerrar documentos, evitando segmentation faults
        game.shutdownUI();
        
        Engine::Physics::Get().Shutdown();
        Engine::AudioManager::Get().shutdown();

    }catch(const std::exception& e)
    {

    }
    
    return EXIT_SUCCESS;
}
