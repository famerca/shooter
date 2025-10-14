#include <iostream>
#include <string>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Mesh.hpp>
#include <Shader.hpp>
#include <Window.hpp>
#include <Model.hpp>
#include <DirectionalLight.hpp>

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

        //specify_vertices();
        Model dado{"dado.fbx"};
        Model suzanne{"suzanne.fbx"};
        Model sphere{"sphere.fbx"};

        DirectionalLight light(1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.5f);

        float current_angle{0.f};

        std::shared_ptr<Context> context = Context::create(main_window->get_aspect_ratio());

        context->addShader("shader.vert", "shader.frag");

        light.bindShader(context->get_shader_list(0)->get_uniform_directional_light());

        sphere.setContext(context);
        suzanne.setContext(context);
        
        sphere.translate(-1.f, 0.f, -2.5f);
        sphere.scale(0.4f, 0.4f, 0.4f);
        suzanne.translate(1.f, 0.f, -2.5f);
        suzanne.scale(0.4f, 0.4f, 0.4f);

        
        while (!main_window->should_be_closed())
        {
            // Get and handle user input events
            glfwPollEvents();

            current_angle += 0.5f;

            if (current_angle >= 360.f)
            {
                current_angle = 0.f;
            }

            // Clear the window
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            suzanne.rotate(Model::X, current_angle);
            sphere.rotate(Model::X, current_angle);

            
            //Draw 
            suzanne.render();
            light.render();
            sphere.render();
            


            glUseProgram(0);

            main_window->swap_buffers();
        }

    }catch(const std::exception& e)
    {

    }
    
    return EXIT_SUCCESS;
}
