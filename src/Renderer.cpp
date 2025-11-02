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
}

void Renderer::start(std::shared_ptr<Scene> scene)
{
    while (running && !scene->getWindow()->should_be_closed())
    {
        this->render(scene->getDirLight());
    }
}

void Renderer::render(std::shared_ptr<DirectionalLight> dirLight)
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

void Renderer::render(std::shared_ptr<CameraComponent> camera)
{
    if(camera != nullptr && camera->isChanged())
    {
        auto renderData = camera->renderData();
        glUniformMatrix4fv(currentShader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(renderData));
    }
}

void Renderer::stop()
{
    running = false;
}       

void Renderer::clear()
{
    
}  