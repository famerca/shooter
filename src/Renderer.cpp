#include "Renderer.hpp"
#include "Input.hpp"


Renderer::Renderer()
{
    fs::path vertex_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.vert";
    fs::path fragment_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "shader.frag";

    shaders.push_back(Shader::create_from_files(vertex_shader_path, fragment_shader_path));
    
    // Cargar shader PBR
    fs::path pbr_vertex_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "pbr.vert";
    fs::path pbr_fragment_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / "pbr.frag";
    auto pbr_shader = Shader::create_from_files(pbr_vertex_shader_path, pbr_fragment_shader_path);
    if (pbr_shader && pbr_shader->get_program_id() != 0)
    {
        shaders.push_back(pbr_shader);
        std::cout << "Shader PBR cargado exitosamente" << std::endl;
    }
    else
    {
        std::cerr << "Error: No se pudo cargar el shader PBR" << std::endl;
        shaders.push_back(nullptr); // Placeholder para mantener el índice
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
        scene->skyBox->set_view(scene->activeCamera->getViewMatrix());
        this->renderSkyBox(scene->getSkyBox(),scene->activeCamera);
        this->currentShader->use();

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
            this->renderObject(object, scene);
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

void Renderer::renderObject(std::shared_ptr<GameObject> object, std::shared_ptr<Scene> scene)
{
    if(object != nullptr && object->isVisible())
    {
        glm::mat4 model = object->getTransform()->getModelMatrix();
        glUniformMatrix4fv(currentShader->get_uniform_model_id(), 1, GL_FALSE, glm::value_ptr(model));
        for (std::shared_ptr<Component> component : object->getComponents())
        {
            if(component->getType() == Component::Type::Model)
            {
                renderModel(std::static_pointer_cast<ModelComponent>(component), scene);
            }
        }
    }
}

void Renderer::renderModel(std::shared_ptr<ModelComponent> model, std::shared_ptr<Scene> scene)
{
    auto model_ptr = model->getModel();
    if (!model_ptr) 
    {
        return;
    }
    
    // Obtener todos los meshes del modelo
    auto meshes = model_ptr->getMeshes();
    if (meshes.empty()) 
    {
        return;
    }
    
    // Verificar si algún mesh tiene texturas PBR
    bool hasPBRTextures = false;
    for (const auto& mesh : meshes)
    {
        if (!mesh) continue;
        const auto& textures = mesh->get_textures();
        for (const auto& tex : textures)
        {
            if (tex.type == "texture_albedo" || tex.type == "texture_metallic" || 
                tex.type == "texture_roughness" || tex.type == "texture_normal" || 
                tex.type == "texture_ao")
            {
                hasPBRTextures = true;
                break;
            }
        }
        if (hasPBRTextures) break;
    }
    
    // Usar PBR si hay texturas PBR y el shader está disponible
    bool usePBR = hasPBRTextures && shaders.size() > 1 && shaders[1] != nullptr && shaders[1]->get_program_id() != 0;
    
    if (usePBR)
    {
        // Usar shader PBR
        auto pbrShader = shaders[1];
        pbrShader->use();
        
        // Configurar matrices
        glm::mat4 modelMatrix = model->getOwner()->getTransform()->getModelMatrix();
        GLint modelLoc = glGetUniformLocation(pbrShader->get_program_id(), "model");
        GLint viewLoc = glGetUniformLocation(pbrShader->get_program_id(), "view");
        GLint projLoc = glGetUniformLocation(pbrShader->get_program_id(), "projection");
        
        if (modelLoc != -1)
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        
        // Obtener matrices de la cámara activa
        auto camera = scene->getCamera();
        if (camera)
        {
            glm::mat4 view = camera->getViewMatrix();
            glm::mat4 projection = camera->getProjectionMatrix();
            
            if (viewLoc != -1)
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            
            if (projLoc != -1)
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
            // Obtener posición de la cámara
            GLint camPosLoc = glGetUniformLocation(pbrShader->get_program_id(), "camPos");
            if (camPosLoc != -1)
            {
                glm::mat4 invView = glm::inverse(view);
                glm::vec3 camPos = glm::vec3(invView[3]);
                glUniform3f(camPosLoc, camPos.x, camPos.y, camPos.z);
            }
        }
        
        // Configurar luces
        GLint lightPosLoc = glGetUniformLocation(pbrShader->get_program_id(), "lightPositions");
        GLint lightColorLoc = glGetUniformLocation(pbrShader->get_program_id(), "lightColors");
        
        if (lightPosLoc != -1 && lightColorLoc != -1)
        {
            // Valores estándar para luces PBR (distancias moderadas, intensidades típicas)
            glm::vec3 lightPositions[4] = {
                glm::vec3(10.0f, 10.0f, 10.0f),
                glm::vec3(-10.0f, 10.0f, 10.0f),
                glm::vec3(10.0f, -10.0f, 10.0f),
                glm::vec3(-10.0f, -10.0f, 10.0f)
            };
            // Intensidad estándar para PBR (típicamente 100-300)
            glm::vec3 lightColors[4] = {
                glm::vec3(150.0f, 150.0f, 150.0f),
                glm::vec3(150.0f, 150.0f, 150.0f),
                glm::vec3(150.0f, 150.0f, 150.0f),
                glm::vec3(150.0f, 150.0f, 150.0f)
            };
            
            glUniform3fv(lightPosLoc, 4, glm::value_ptr(lightPositions[0]));
            glUniform3fv(lightColorLoc, 4, glm::value_ptr(lightColors[0]));
        }
        
        // Renderizar todos los meshes con shader PBR
        for (const auto& mesh : meshes)
        {
            if (!mesh) continue;
            mesh->render(pbrShader);
        }
        
        // Volver al shader por defecto
        if (currentShader)
            currentShader->use();
    }
    else
    {
        // Usar método de renderizado estándar
        if (currentShader)
        {
            GLint diffuseTexLoc = currentShader->get_uniform_diffuse_texture_id();
            if (diffuseTexLoc != -1)
            {
                model_ptr->render(diffuseTexLoc);
            }
            else
            {
                // Si no hay uniform de textura, renderizar meshes directamente
                for (const auto& mesh : meshes)
                {
                    if (mesh) mesh->render();
                }
            }
        }
        else
        {
            // Fallback: renderizar directamente sin shader
            for (const auto& mesh : meshes)
            {
                if (mesh) mesh->render();
            }
        }
    }
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
        glDepthMask(GL_FALSE);
    
        sky_box->shader->use();

        auto view = sky_box->get_view();
        auto projection = camera->getProjectionMatrix();
    
        glUniformMatrix4fv(sky_box->shader->get_uniform_view_id(), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(sky_box->shader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(projection));

        sky_box->render();
    
        glDepthMask(GL_TRUE);

    }

}