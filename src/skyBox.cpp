#include <SkyBox.hpp>
#include <BSlogger.hpp>
#include <stb_image.h>
#include "Path.hpp"

namespace Engine
{

SkyBox::SkyBox(const std::string& dir, const std::vector<std::filesystem::path>& face_filenames) noexcept
{
    // Texture setup
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    int width{0};
    int height{0};
    int channels{0};

    // Para cubemaps, no debemos voltear verticalmente las texturas
    stbi_set_flip_vertically_on_load(false);

    for (size_t i = 0; i < face_filenames.size(); ++i)
    {
        auto file_path = TEXTURES_PATH / dir / face_filenames[i];
        unsigned char* tex_data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);

        if (!tex_data)
        {
            LOG_INIT_CERR();
            log(LOG_ERR) << "Failed to load skybox texture: " << file_path << "\n";
            continue; // Continuar con la siguiente textura en lugar de retornar
        }

        // Determinar el formato basado en el número de canales
        GLenum format = GL_RGB;
        GLenum internal_format = GL_RGB;
        if (channels == 4)
        {
            format = GL_RGBA;
            internal_format = GL_RGBA;
        }
        else if (channels == 1)
        {
            format = GL_RED;
            internal_format = GL_RED;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data);

        stbi_image_free(tex_data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Mesh setup
    std::vector<unsigned int> indices{{
        // front
        0, 1, 2,
        2, 1, 3,
        // right
        2, 3, 5,
        5, 3, 7,
        // back
        5, 7, 4,
        4, 7, 6,
        // left
        4, 6, 0,
        0, 6, 1,
        // top
        4, 0, 5,
        5, 0, 2,
        // bottom
        1, 6, 3,
        3, 6, 7
    }};

     std::vector<Vertex> vertices = {
        //   {posición, normal, texCoords}
        
        Vertex{ glm::vec3(-1.f, 1.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f) },
        Vertex{ glm::vec3(-1.f, -1.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f) },
        Vertex{ glm::vec3(1.f, 1.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f) },
        Vertex{ glm::vec3(1.f, -1.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f) },
        Vertex{ glm::vec3(-1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f) },
        Vertex{ glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f) },
        Vertex{ glm::vec3(-1.f, -1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f) },
        Vertex{ glm::vec3(1.f, -1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.f, 0.f) }
    };

    mesh = Mesh::create(vertices, indices);
}

SkyBox::~SkyBox()
{
    if (texture_id)
    {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
}

void SkyBox::render() const noexcept
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    mesh->render();
}


const glm::mat4& SkyBox::get_view() const noexcept
{
    return skybox_view;
}


void SkyBox::set_view(const glm::mat4& view) noexcept
{
    // Removing translation
    skybox_view = glm::mat4(glm::mat3(view));
}

}