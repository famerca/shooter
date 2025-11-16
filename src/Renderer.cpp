#include "Renderer.hpp"
#include "Input.hpp"
#include "Physics.hpp"


Renderer::Renderer()
{
    fs::path path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders";

    shaders.push_back(Shader::create_from_files(path / "shader.vert", path / "shader.frag"));
    shaders.push_back(Shader::create_from_files(path / "skybox.vert", path / "skybox.frag"));
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
        fs::path path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders";
        hitboxRenderer = std::make_shared<Engine::HitboxRenderer>(Shader::create_from_files(path / "line.vert", path / "line.frag"));
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

        if(Engine::Physics::Get().IsInitialized())
            Engine::Physics::Get().Step(delta_time);
            
        scene->update(delta_time);

        this->useShader(Shader::LIST::BASE);

        if(scene->window->getInput() != nullptr)
        {
            scene->window->getInput()->poll(delta_time);
        }
        //render Direction Light
        this->renderDirLight(scene->DirLight);
        //render active camera
        this->renderCamera(scene->activeCamera);

        //render objects
        for (std::shared_ptr<GameObject> object : scene->Objects)
        {
            this->renderObject(object);
        }

        RenderDebug(scene->activeCamera);

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
        auto matrix = camera->getProjectionMatrix();
        auto view = camera->getViewMatrix();
        hitboxRenderer->SetViewProjectionMatrix(view, matrix);
        Engine::Physics::Get().DrawBodies(drawSetting, hitboxRenderer.get());
     
    #endif
}

void Renderer::renderObject(std::shared_ptr<GameObject> object)
{
    if(object != nullptr && object->isVisible())
    {
        glm::mat4 model = object->getTransform()->getModelMatrix();
        glUniformMatrix4fv(currentShader->get_uniform_model_id(), 1, GL_FALSE, glm::value_ptr(model));
        for (std::shared_ptr<Component> component : object->getComponents())
        {
            if(component->getType() == Component::Type::Model)
            {
                renderModel(std::static_pointer_cast<ModelComponent>(component));
            }
        }
    }
}

void Renderer::renderModel(std::shared_ptr<ModelComponent> model)
{
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

void Renderer::calcDeltaTime()
{
    GLdouble current_time = glfwGetTime();
    delta_time = static_cast<GLfloat>(current_time - last_frame_time);
    last_frame_time = current_time;
}

void Renderer::renderSkyBox(std::shared_ptr<SkyBox>  sky_box, std::shared_ptr<CameraComponent> camera)
{
    if(sky_box != nullptr)
    {
        if(!camera->isRenderd())
            sky_box->set_view(camera->getViewMatrix());
            
        glDepthMask(GL_FALSE);
    
        this->useShader(Shader::LIST::SKYBOX);

        auto view = sky_box->get_view();
        auto projection = camera->getProjectionMatrix();
    
        glUniformMatrix4fv(this->currentShader->get_uniform_view_id(), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(this->currentShader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(projection));

        sky_box->render();
    
        glDepthMask(GL_TRUE);

    }

}