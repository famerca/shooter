#include "Renderer.hpp"

Renderer::Renderer()
{
    fs::path vertex_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.vert";
    fs::path fragment_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.frag";

    shaders.push_back(Shader::create_from_files(vertex_shader_path, fragment_shader_path));
    currentShader = nullptr;
}

Renderer::~Renderer()
{   
}

void Renderer::init()
{
    shaders[0]->use();
    currentShader = shaders[0];
    running = true;
}

void Renderer::render(std::shared_ptr<Scene> scene)
{
    scene->update(1.f);
    while (running && !scene->getWindow()->should_be_closed())
    {
        //clear the window
        scene->window->clear();
        //render Direction Light
        this->renderDirLight(scene->DirLight);
        //render active camera
        this->renderCamera(scene->activeCamera);

        //render objects
        for (std::shared_ptr<GameObject> object : scene->Objects)
        {
            this->renderObject(object);
        }

        scene->window->swap_buffers();
    }

    glUseProgram(0);
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
    if(camera != nullptr && camera->isChanged())
    {
        auto matrix = camera->getProjectionMatrix();
        glUniformMatrix4fv(currentShader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(matrix));
    }
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
    
}  