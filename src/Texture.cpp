#include "Texture.hpp"
#include <iostream>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Texture::~Texture()
{
    clear();
}

std::shared_ptr<Texture> Texture::create_from_file(const std::filesystem::path& texture_path) noexcept
{
    auto texture = std::make_shared<Texture>();
    
    // Generar textura
    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
    
    // Configurar parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Cargar imagen
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texture_path.string().c_str(), &texture->width, &texture->height, &texture->channels, 0);
    
    if (data)
    {
        GLenum format = GL_RGB;
        if (texture->channels == 4)
            format = GL_RGBA;
        else if (texture->channels == 1)
            format = GL_RED;
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Error al cargar la textura: " << texture_path << std::endl;
        stbi_image_free(data);
        return nullptr;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture;
}

void Texture::bind(unsigned int slot) const noexcept
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Texture::unbind() const noexcept
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::clear() noexcept
{
    if (texture_id != 0)
    {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
} 