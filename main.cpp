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

namespace fs = std::filesystem;

struct Data
{
    static std::vector<std::shared_ptr<Mesh>> mesh_list;
    static std::vector<std::shared_ptr<Shader>> shader_list;
    static const fs::path shaders_path;
    static const fs::path vertex_shader_path;
    static const fs::path fragment_shader_path;
};

std::vector<std::shared_ptr<Mesh>> Data::mesh_list{};
std::vector<std::shared_ptr<Shader>> Data::shader_list{};

const fs::path Data::shaders_path{fs::path{__FILE__}.parent_path() / "shaders"};
const fs::path Data::vertex_shader_path{Data::shaders_path / "shader.vert"};
const fs::path Data::fragment_shader_path{Data::shaders_path / "shader.frag"};

float to_radian(float degrees)
{
    return degrees * M_PI / 180.f;
}

void specify_vertices() noexcept
{
    //Pyramid
    std::vector<GLfloat> positions{
        // Base (plano Y=-1.0f)
        -1.0f, -1.0f,  1.0f,  // 0: Abajo a la izquierda (frontal)
         1.0f, -1.0f,  1.0f,  // 1: Abajo a la derecha (frontal)
         1.0f, -1.0f, -1.0f,  // 2: Abajo a la derecha (trasera)
        -1.0f, -1.0f, -1.0f,  // 3: Abajo a la izquierda (trasera)
    
        // Ápice
         0.0f,  1.0f,  0.0f   // 4: Ápice
    };
    
    // Índices para dibujar las caras de la pirámide de base cuadrada
    // Cada conjunto de 3 índices forma un triángulo.
    std::vector<unsigned int> indices{
        // Cara frontal (triángulo)
        0, 1, 4,
    
        // Cara derecha (triángulo)
        1, 2, 4,
    
        // Cara trasera (triángulo)
        2, 3, 4,
    
        // Cara izquierda (triángulo)
        3, 0, 4,
    
        // Base (dos triángulos para formar el cuadrado)
        0, 2, 1, // Triángulo inferior-izquierdo de la base
        0, 3, 2  // Triángulo superior-derecho de la base
    };

    std::vector<Vertex> vertices;
    // Convertir el array de GLfloat a un vector de Vertex
    for (size_t i = 0; i < positions.size(); i += 3) {
        Vertex v;
        v.position = glm::vec3(positions[i], positions[i+1], positions[i+2]);
        v.normal = glm::vec3(0.0f, 0.0f, 0.0f);   // Normales por defecto (sin iluminación)
        v.texCoords = glm::vec2(0.0f, 0.0f);     // Coordenadas de textura por defecto
        vertices.push_back(v);
    }
    

    Data::mesh_list.push_back(Mesh::create(vertices, indices));

    //Cube
    positions = {
        // Cara inferior (Y = -1.0f)
        -1.0f, -1.0f,  1.0f,  // 0: Abajo-Izquierda-Frontal
         1.0f, -1.0f,  1.0f,  // 1: Abajo-Derecha-Frontal
         1.0f, -1.0f, -1.0f,  // 2: Abajo-Derecha-Trasera
        -1.0f, -1.0f, -1.0f,  // 3: Abajo-Izquierda-Trasera
    
        // Cara superior (Y = 1.0f)
        -1.0f,  1.0f,  1.0f,  // 4: Arriba-Izquierda-Frontal
         1.0f,  1.0f,  1.0f,  // 5: Arriba-Derecha-Frontal
         1.0f,  1.0f, -1.0f,  // 6: Arriba-Derecha-Trasera
        -1.0f,  1.0f, -1.0f   // 7: Arriba-Izquierda-Trasera
    };
    
    // Índices para dibujar las 6 caras del cubo, cada una con 2 triángulos.
    // Asegurándose de que las normales de las caras apunten hacia afuera (sentido anti-horario).
    indices = {
        // Cara frontal (+Z)
        0, 1, 5,  // Triángulo 1
        0, 5, 4,  // Triángulo 2
    
        // Cara trasera (-Z)
        3, 2, 6,  // Triángulo 1
        3, 6, 7,  // Triángulo 2
    
        // Cara derecha (+X)
        1, 2, 6,  // Triángulo 1
        1, 6, 5,  // Triángulo 2
    
        // Cara izquierda (-X)
        0, 3, 7,  // Triángulo 1
        0, 7, 4,  // Triángulo 2
    
        // Cara superior (+Y)
        4, 5, 6,  // Triángulo 1
        4, 6, 7,  // Triángulo 2
    
        // Cara inferior (-Y)
        0, 3, 2,  // Triángulo 1
        0, 2, 1   // Triángulo 2
    };
    
    vertices.clear();
    // Convertir el array de GLfloat a un vector de Vertex
    for (size_t i = 0; i < positions.size(); i += 3) {
        Vertex v;
        v.position = glm::vec3(positions[i], positions[i+1], positions[i+2]);
        v.normal = glm::vec3(0.0f, 0.0f, 0.0f);   // Normales por defecto (sin iluminación)
        v.texCoords = glm::vec2(0.0f, 0.0f);     // Coordenadas de textura por defecto
        vertices.push_back(v);
    }
    

    Data::mesh_list.push_back(Mesh::create(vertices, indices));
}

void create_shaders_program() noexcept
{
    Data::shader_list.push_back(Shader::create_from_files(Data::vertex_shader_path, Data::fragment_shader_path));
}

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
        create_shaders_program();

        float current_angle{0.f};

        glm::mat4 projection = glm::perspective(45.f, main_window->get_aspect_ratio(), 0.1f, 100.f);
        
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

            Data::shader_list[0]->use();

            glm::mat4 modelTriangle{1.f};
            glm::mat4 modelCube{1.f};
            
            glUniformMatrix4fv(Data::shader_list[0]->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(projection));

            modelTriangle = glm::translate(modelTriangle, glm::vec3{-1.0f, 0.f, -2.5f});
            modelTriangle = glm::rotate(modelTriangle, to_radian(current_angle), glm::vec3{0.f, 1.f, 0.f});
            modelTriangle = glm::scale(modelTriangle, glm::vec3{0.4f, 0.4f, 0.4f});

            modelCube = glm::translate(modelCube, glm::vec3{1.0f, 0.f, -2.5f});
            modelCube = glm::rotate(modelCube, to_radian(current_angle), glm::vec3{0.f, 1.f, 0.f});
            modelCube = glm::scale(modelCube, glm::vec3{0.4f, 0.4f, 0.4f});
            
            //Draw meshes 

            glUniformMatrix4fv(Data::shader_list[0]->get_uniform_model_id(), 1, GL_FALSE, glm::value_ptr(modelTriangle));
            suzanne.render();
            
            glUniformMatrix4fv(Data::shader_list[0]->get_uniform_model_id(), 1, GL_FALSE, glm::value_ptr(modelCube));
            dado.render();

            glUseProgram(0);

            main_window->swap_buffers();
        }

    }catch(const std::exception& e)
    {

    }
    
    return EXIT_SUCCESS;
}
