#include "Renderer.hpp"
#include "Input.hpp"

Renderer::Renderer()
{
    fs::path vertex_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.vert";
    fs::path fragment_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.frag";

    shaders.push_back(Shader::create_from_files(vertex_shader_path, fragment_shader_path));
    
    fs::path skybox_vert_path = fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "skybox.vert";
    fs::path skybox_frag_path = fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "skybox.frag";
    skybox_shader = Shader::create_from_files(skybox_vert_path, skybox_frag_path);
    
    if (skybox_shader == nullptr || skybox_shader->get_program_id() == 0)
    {
        std::cerr << "Warning: Skybox shader failed to load" << std::endl;
    }
    
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
    last_frame_time = glfwGetTime();
}

void Renderer::render(std::shared_ptr<Scene> scene)
{
    while (running && !scene->getWindow()->should_be_closed())
    {
        //clear the window
        scene->window->clear();
        calcDeltaTime();

        scene->update(delta_time);
        
        if(scene->window->getInput() != nullptr)
        {
            scene->window->getInput()->poll(delta_time);
        }
        
       // if (scene->skybox != nullptr && scene->activeCamera != nullptr)
        //{
            this->renderSkybox(scene->skybox, scene->activeCamera);
        //}

        currentShader->use();

        this->renderDirLight(scene->DirLight);
        this->renderCamera(scene->activeCamera);

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

GLfloat Renderer::getDeltaTime() const noexcept
{
    return delta_time;
}

void Renderer::renderSkybox(std::shared_ptr<Skybox> skybox, std::shared_ptr<CameraComponent> camera)
{
    if (skybox == nullptr || camera == nullptr || skybox_shader == nullptr) return;

    GLboolean cull_enabled;
    glGetBooleanv(GL_CULL_FACE, &cull_enabled);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    skybox_shader->use();

    GLuint program_id = skybox_shader->get_program_id();
    
    if (program_id == 0)
    {
        return;
    }

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    GLuint proj_loc = glGetUniformLocation(program_id, "projection");
    GLuint view_loc = glGetUniformLocation(program_id, "view");
    GLuint use_tex_loc = glGetUniformLocation(program_id, "use_texture");
    GLuint top_color_loc = glGetUniformLocation(program_id, "top_color");
    GLuint bottom_color_loc = glGetUniformLocation(program_id, "bottom_color");
    GLuint skybox_sampler_loc = glGetUniformLocation(program_id, "skybox");

    if (proj_loc == -1 || view_loc == -1)
    {
        return;
    }

    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
    
    glm::mat4 view_no_translation = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_no_translation));

    if (skybox->has_texture())
    {
        if (use_tex_loc != -1) glUniform1i(use_tex_loc, 1);
        if (skybox_sampler_loc != -1) glUniform1i(skybox_sampler_loc, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->get_cubemap_texture());
    }
    else
    {
        if (use_tex_loc != -1) glUniform1i(use_tex_loc, 0);
        glm::vec3 top_color(0.5f, 0.7f, 1.0f);
        glm::vec3 bottom_color(0.2f, 0.3f, 0.5f);
        
        if (top_color_loc == -1 || bottom_color_loc == -1)
        {
            return;
        }
        
        glUniform3fv(top_color_loc, 1, glm::value_ptr(top_color));
        glUniform3fv(bottom_color_loc, 1, glm::value_ptr(bottom_color));
        
        if (skybox_sampler_loc != -1)
        {
            if (dummy_cubemap == 0)
            {
                createDummyCubemap();
            }
            glUniform1i(skybox_sampler_loc, 0);
        }
        
        if (dummy_cubemap == 0)
        {
            createDummyCubemap();
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dummy_cubemap);
    }

    skybox->render();

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    if (cull_enabled)
    {
        glEnable(GL_CULL_FACE);
    }
}

void Renderer::createDummyCubemap()
{
    glGenTextures(1, &dummy_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dummy_cubemap);
    unsigned char dummy_color[3] = {0, 0, 0};
    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, dummy_color);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Renderer::calcDeltaTime()
{
    GLdouble current_time = glfwGetTime();
    delta_time = static_cast<GLfloat>(current_time - last_frame_time);
    last_frame_time = current_time;
}