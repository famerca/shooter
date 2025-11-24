#include "Renderer.hpp"
#include "Input.hpp"
#include "Physics.hpp"
#include "Path.hpp"
#include "RmlUiInterface.hpp"
#include "Utils.hpp"
#include <iostream>

namespace Engine
{

Renderer::Renderer()
{
    

    shaders.push_back(Shader::create_from_files(SHADERS_PATH / "shader.vert", SHADERS_PATH / "shader.frag"));
    shaders.push_back(Shader::create_from_files(SHADERS_PATH / "skybox.vert", SHADERS_PATH / "skybox.frag"));
    hitboxRenderer = nullptr;

    currentShader = nullptr;
    activeShade = Shader::LIST::BASE;
}

Renderer::~Renderer()
{   
}

void Renderer::debug()
{
    #ifdef JPH_DEBUG_RENDERER
        
        hitboxRenderer = std::make_shared<Engine::HitboxRenderer>(Shader::create_from_files(SHADERS_PATH / "line.vert", SHADERS_PATH / "line.frag"));
        JPH::BodyManager::DrawSettings bodyDrawSettings;
        // 1. Dibuja las formas de colisión (hitbox)
        bodyDrawSettings.mDrawShape = true; 

        // 2. Fuerzas que las formas se dibujen como wireframe (contorno)
        bodyDrawSettings.mDrawShapeWireframe = true;
        drawSetting = bodyDrawSettings;
    #endif

}

void Renderer::useShader(Shader::LIST shader)
{
    if(shader == activeShade)
        return;

    currentShader = shaders[static_cast<int>(shader)];
    currentShader->use();
    activeShade = shader;
}

void Renderer::init()
{
    running = true;
    last_frame_time = glfwGetTime();
}

void Renderer::render(std::shared_ptr<Scene> scene)
{
    while (running && !scene->getWindow()->should_be_closed())
    {
        //clear the window
        scene->window->clear();
        this->renderSkyBox(scene->getSkyBox(),scene->activeCamera);

        calcDeltaTime();

        // Procesar input antes de verificar pausa (para detectar ESC)
        if(scene->window->getInput() != nullptr)
        {
            scene->window->getInput()->poll(delta_time);
            
            // Detectar tecla ESC para pausar/reanudar
            if (rmlui_interface && rmlui_interface->IsInitialized())
            {
                auto input = scene->window->getInput();
                static bool last_esc_state = false;
                bool current_esc_state = input->is_key_pressed(GLFW_KEY_ESCAPE);
                
                // Detectar transición de false a true (tecla presionada)
                if (current_esc_state && !last_esc_state)
                {
                    // Si el menú principal está visible, no hacer nada (ya está pausado)
                    if (!rmlui_interface->IsMainMenuVisible())
                    {
                        // Si el menú de pausa está visible, ocultarlo (reanudar)
                        if (rmlui_interface->IsPauseMenuVisible())
                        {
                            rmlui_interface->HidePauseMenu();
                        }
                        // Si no está visible, mostrarlo (pausar)
                        else
                        {
                            rmlui_interface->ShowPauseMenu();
                        }
                    }
                }
                
                last_esc_state = current_esc_state;
            }
        }

        // Pausar física y actualización de escena si algún menú está visible
        bool game_paused = (rmlui_interface && rmlui_interface->IsInitialized() && 
                           (rmlui_interface->IsMainMenuVisible() || rmlui_interface->IsPauseMenuVisible()));
        
        if (!game_paused)
        {
            if(Engine::Physics::Get().IsInitialized())
                Engine::Physics::Get().Step(delta_time);
                
            scene->update(delta_time);
        }

        this->useShader(Shader::LIST::BASE);
        
        // Procesar input de RmlUi (debe hacerse antes de Update())
        if (rmlui_interface && rmlui_interface->IsInitialized())
        {
            auto input = scene->window->getInput();
            GLFWwindow* glfw_window = scene->window->getGLFWWindow();
            
            if (input && glfw_window)
            {
                // Obtener posición del mouse directamente de GLFW (coordenadas de ventana)
                double mouse_x_window, mouse_y_window;
                glfwGetCursorPos(glfw_window, &mouse_x_window, &mouse_y_window);
                
                // Obtener tamaño del framebuffer (puede ser diferente en pantallas de alta resolución)
                int framebuffer_width, framebuffer_height;
                glfwGetFramebufferSize(glfw_window, &framebuffer_width, &framebuffer_height);
                
                // Obtener tamaño de la ventana
                int window_width, window_height;
                glfwGetWindowSize(glfw_window, &window_width, &window_height);
                
                // Convertir coordenadas de ventana a coordenadas de framebuffer
                // Esto es necesario para pantallas de alta resolución (Retina, etc.)
                double scale_x = static_cast<double>(framebuffer_width) / static_cast<double>(window_width);
                double scale_y = static_cast<double>(framebuffer_height) / static_cast<double>(window_height);
                
                int mouse_x = static_cast<int>(mouse_x_window * scale_x);
                int mouse_y = static_cast<int>(mouse_y_window * scale_y);
                
                // Procesar movimiento del mouse en RmlUi (usar coordenadas de framebuffer)
                rmlui_interface->GetContext()->ProcessMouseMove(
                    mouse_x, 
                    mouse_y, 
                    0  // key_modifier_state (0 = sin modificadores)
                );
                
                // Procesar clicks del mouse en RmlUi
                // Usar estado estático para detectar transiciones (press/release)
                static bool last_mouse_state = false;
                bool current_mouse_state = input->is_mouse_button_pressed(0); // 0 = GLFW_MOUSE_BUTTON_LEFT
                
                if (current_mouse_state && !last_mouse_state)
                {
                    // Botón presionado - pasar coordenadas de framebuffer
                    std::cout << "Renderer: Mouse button DOWN en ventana(" << mouse_x_window << ", " << mouse_y_window 
                              << ") framebuffer(" << mouse_x << ", " << mouse_y << ")" << std::endl;
                    rmlui_interface->GetContext()->ProcessMouseButtonDown(0, 0);
                }
                else if (!current_mouse_state && last_mouse_state)
                {
                    // Botón liberado - pasar coordenadas de framebuffer
                    std::cout << "Renderer: Mouse button UP en ventana(" << mouse_x_window << ", " << mouse_y_window 
                              << ") framebuffer(" << mouse_x << ", " << mouse_y << ")" << std::endl;
                    rmlui_interface->GetContext()->ProcessMouseButtonUp(0, 0);
                }
                
                last_mouse_state = current_mouse_state;
            }
            
            // Actualizar RmlUi después de procesar input (según documentación)
            // Esto procesa los eventos y actualiza el estado interno
            rmlui_interface->Update();
        }
        
        //render Direction Light
        this->renderDirLight(scene->DirLight);
        //render active camera
        this->renderCamera(scene->activeCamera);

        //render objects
        int i = 0;
        for (std::shared_ptr<GameObject> object : scene->Objects)
        {
            std::cout << "Rendering object " << i++ << std::endl;
            this->renderObject(object);
        }

        RenderDebug(scene->activeCamera);

        // Renderizar RmlUi si está disponible (después de todo el renderizado 3D)
        // Update ya se llamó antes, solo renderizar
        if (rmlui_interface && rmlui_interface->IsInitialized())
        {
            rmlui_interface->Render();
        }

        scene->window->swap_buffers();
    }

    clear();
}

void Renderer::renderDirLight(std::shared_ptr<DirectionalLight> dirLight)
{
    if(dirLight != nullptr && dirLight->isChanged())
    {
        
        auto renderData = dirLight->renderData();
        DirectionalLightUniforms uniforms = currentShader->get_uniform_directional_light();
        
        glUniform1f(uniforms.diffuse_intensity_id, std::get<0>(renderData));
        glUniform3f(uniforms.color_id, std::get<1>(renderData)[0], std::get<1>(renderData)[1], std::get<1>(renderData)[2]);
        glUniform3f(uniforms.direction_id, std::get<2>(renderData)[0], std::get<2>(renderData)[1], std::get<2>(renderData)[2]);

    }

}

void Renderer::renderCamera(std::shared_ptr<CameraComponent> camera)
{
    if(camera != nullptr && !camera->isRenderd())
    {
        auto matrix = camera->getProjectionMatrix();
        auto view = camera->getViewMatrix();
        glUniformMatrix4fv(currentShader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(currentShader->get_uniform_view_id(), 1, GL_FALSE, glm::value_ptr(view));

    }
    
}


void Renderer::RenderDebug(std::shared_ptr<CameraComponent> camera)
{
    #ifdef JPH_DEBUG_RENDERER
        if(hitboxRenderer)
        {
            auto matrix = camera->getProjectionMatrix();
            auto view = camera->getViewMatrix();
            hitboxRenderer->SetViewProjectionMatrix(view, matrix);
            Engine::Physics::Get().DrawBodies(drawSetting, hitboxRenderer.get());
        }
     
    #endif
}

void Renderer::renderObject(std::shared_ptr<GameObject> object)
{
    if(object != nullptr && object->isVisible())
    {
        auto model = object->getTransform()->getModelMatrix();
        std::cout << "Body Postion: " << object->getBody()->GetPosition() << std::endl;
        Utils::printMat4(model);

        for (std::shared_ptr<Component> component : object->getComponents())
        {
            if(component->getType() == Component::Type::Model)
            {
                renderModel(std::static_pointer_cast<ModelComponent>(component), model);
            }
        }
    }
}

void Renderer::renderModel(std::shared_ptr<ModelComponent> model,  glm::mat4 m)
{
    if(model->getModel() == nullptr)
        return;

    if(model->isRelative())
    {
        m = m * model->getModelMatrix(); 
    }

    glUniformMatrix4fv(currentShader->get_uniform_model_id(), 1, GL_FALSE, glm::value_ptr(m));
    model->getModel()->render(currentShader->get_uniform_diffuse_texture_id());
}

void Renderer::stop()
{
    running = false;
}       

void Renderer::clear() noexcept
{
    glUseProgram(0);
}  

GLfloat Renderer::getDeltaTime() const noexcept
{
    return delta_time;
}

void Renderer::setRmlUiInterface(std::shared_ptr<::RmlUiInterface> rmlui) noexcept
{
    rmlui_interface = rmlui;
}

void Renderer::calcDeltaTime()
{
    GLdouble current_time = glfwGetTime();
    delta_time = static_cast<GLfloat>(current_time - last_frame_time);
    last_frame_time = current_time;
}

void Renderer::renderSkyBox(std::shared_ptr<SkyBox>  sky_box, std::shared_ptr<CameraComponent> camera)
{
    if(sky_box != nullptr && camera != nullptr)
    {
        // Siempre actualizar la vista del skybox con la matriz de la cámara (sin traslación)
        // Esto permite que el skybox rote con la cámara
        sky_box->set_view(camera->getViewMatrix());
        
        // Guardar el estado del depth function
        GLint depth_func_was;
        glGetIntegerv(GL_DEPTH_FUNC, &depth_func_was);
            
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL); // Cambiar la función de profundidad para que el skybox siempre se dibuje detrás
    
        this->useShader(Shader::LIST::SKYBOX);

        auto view = sky_box->get_view();
        auto projection = camera->getProjectionMatrix();
    
        glUniformMatrix4fv(this->currentShader->get_uniform_view_id(), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(this->currentShader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(projection));

        // Establecer el uniform del samplerCube skybox ANTES de vincular la textura
        GLuint skybox_uniform = this->currentShader->get_uniform_skybox_id();
        if (skybox_uniform != static_cast<GLuint>(-1))
        {
            glUniform1i(skybox_uniform, 0); // 0 corresponde a GL_TEXTURE0
        }
        
        // Asegurarse de que GL_TEXTURE0 esté activo antes de renderizar
        glActiveTexture(GL_TEXTURE0);

        sky_box->render();
    
        // Restaurar el estado del depth mask y la función de profundidad
        glDepthMask(GL_TRUE);
        glDepthFunc(depth_func_was); // Restaurar la función de profundidad anterior
        
        // Desbindear el cubemap para no interferir con otras texturas
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

}

}