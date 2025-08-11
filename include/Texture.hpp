#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include <GL/glew.h>
#include <stb_image.h>



#ifndef TEXTURE_HPP
#define TEXTURE_HPP

class Model; // Forward declaration

class Texture
{
    friend class Model; // Permitir que Model acceda a miembros privados
    
public:
    Texture() = default;
    
    static std::shared_ptr<Texture> create_from_file(const std::filesystem::path& texture_path) noexcept;
    
    Texture(const Texture& texture) = delete;
    Texture(Texture&& texture) = delete;
    
    ~Texture();
    
    Texture& operator=(const Texture& texture) = delete;
    Texture& operator=(Texture&& texture) = delete;
    
    void bind(unsigned int slot = 0) const noexcept;
    void unbind() const noexcept;
    
    GLuint get_id() const noexcept { return texture_id; }
    int get_width() const noexcept { return width; }
    int get_height() const noexcept { return height; }
    
private:
    void clear() noexcept;
    
    GLuint texture_id{0};
    int width{0};
    int height{0};
    int channels{0};
};

#endif // TEXTURE_HPP 