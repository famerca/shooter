#include "Skybox.hpp"
#include "Context.hpp"
#include <stb_image.h>
#include <iostream>

const float SKYBOX_SIZE = 50.0f;

const float skybox_vertices[] = {
    -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
     SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
     SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,

     SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
     SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
     SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,

    -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
     SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
     SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE
};

Skybox::~Skybox()
{
    clear();
}

std::shared_ptr<Skybox> Skybox::create() noexcept
{
    auto skybox = std::make_shared<Skybox>();
    if (skybox != nullptr)
    {
        skybox->create_cube_mesh();
    }
    return skybox;
}

bool Skybox::load_cubemap(const std::vector<std::filesystem::path>& faces) noexcept
{
    if (faces.size() != 6)
    {
        return false;
    }

    glGenTextures(1, &cubemap_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

    int width, height, nr_channels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char* data = stbi_load(faces[i].string().c_str(), &width, &height, &nr_channels, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (nr_channels == 4)
                format = GL_RGBA;
            else if (nr_channels == 1)
                format = GL_RED;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            stbi_image_free(data);
            glDeleteTextures(1, &cubemap_texture);
            cubemap_texture = 0;
            return false;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

void Skybox::create_cube_mesh() noexcept
{
    if (VAO_id != 0)
    {
        return;
    }

    glGenVertexArrays(1, &VAO_id);
    glBindVertexArray(VAO_id);

    glGenBuffers(1, &VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vertex_count = 36;
}

void Skybox::render() const noexcept
{
    if (VAO_id == 0 || vertex_count == 0)
    {
        return;
    }

    glBindVertexArray(VAO_id);
    if (cubemap_texture != 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
    }
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    glBindVertexArray(0);
}

void Skybox::clear() noexcept
{
    if (VAO_id != 0)
    {
        glDeleteVertexArrays(1, &VAO_id);
        VAO_id = 0;
    }

    if (VBO_id != 0)
    {
        glDeleteBuffers(1, &VBO_id);
        VBO_id = 0;
    }

    if (cubemap_texture != 0)
    {
        glDeleteTextures(1, &cubemap_texture);
        cubemap_texture = 0;
    }

    vertex_count = 0;
}

