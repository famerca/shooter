#include "Renderer.hpp"
#include "Input.hpp"

Renderer::Renderer()
{
    fs::path vertex_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.vert";
    fs::path fragment_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.frag";
    this->sky_box = std::make_shared<SkyBox>(
        fs::path{__FILE__}.parent_path() / "../",
        std::vector<fs::path>{{
            "cupertin-lake_rt.tga",
            "cupertin-lake_lf.tga",
            "cupertin-lake_up.tga",
            "cupertin-lake_dn.tga",
            "cupertin-lake_bk.tga",
            "cupertin-lake_ft.tga",
        }}
    );

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
    last_frame_time = glfwGetTime();
}

void Renderer::render(std::shared_ptr<Scene> scene)
{
    auto matrix = scene->activeCamera->getProjectionMatrix();
    auto view = scene->activeCamera->getViewMatrix();
    render_pass(matrix,view);
    
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
    if(camera != nullptr && !camera->isRenderd())
    {
        auto matrix = camera->getProjectionMatrix();
        auto view = camera->getViewMatrix();
        glUniformMatrix4fv(currentShader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(matrix));
        glUniformMatrix4fv(currentShader->get_uniform_view_id(), 1, GL_FALSE, glm::value_ptr(view));

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

void Renderer::calcDeltaTime()
{
    GLdouble current_time = glfwGetTime();
    delta_time = static_cast<GLfloat>(current_time - last_frame_time);
    last_frame_time = current_time;
}

void Renderer::render_pass(const glm::mat4& projection, const glm::mat4& view) 
{
    //glViewport(0, 0, Data::WIDTH, Data::HEIGHT);

    // Clear the window
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sky_box->render(view, projection);

    
/*
    Data::shader_list[0]->use();
    Data::uniform_model_id = Data::shader_list[0]->get_uniform_model_id();
    Data::uniform_projection_id = Data::shader_list[0]->get_uniform_projection_id();
    Data::uniform_view_id = Data::shader_list[0]->get_uniform_view_id();
    Data::uniform_eye_position_id = Data::shader_list[0]->get_uniform_eye_position_id();
    Data::uniform_specular_intensity_id = Data::shader_list[0]->get_uniform_specular_intensity_id();
    Data::uniform_shininess_id = Data::shader_list[0]->get_uniform_specular_shininess_id();

    glUniformMatrix4fv(Data::shader_list[0]->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(Data::shader_list[0]->get_uniform_view_id(), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3f(Data::shader_list[0]->get_uniform_eye_position_id(), Data::camera->get_position().x, Data::camera->get_position().y, Data::camera->get_position().z);

    Data::shader_list[0]->set_directional_light(Data::main_light);
    Data::shader_list[0]->set_point_lights(Data::point_lights, 3, 0);
    Data::shader_list[0]->set_spot_lights(Data::spot_lights, 3 + Data::point_lights.size(), Data::point_lights.size());
    Data::shader_list[0]->set_directional_light_space_transform(Data::main_light->get_light_transform());

    Data::main_light->get_shadow_map()->read(GL_TEXTURE2);
    Data::shader_list[0]->set_texture(1);
    Data::shader_list[0]->set_directional_shadow_map(2);

    auto lower_light = Data::camera->get_position();
    lower_light.y -= 0.3f;
    //Data::spot_lights[0]->set(lower_light, Data::camera->get_direction());

    render_scene();

    */
}