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
    
    static std::shared_ptr<Texture> create_from_file(const std::filesystem::path& texture_path, const std::string& type = "", const std::string& path = "") noexcept;
    
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
    
    // Getters para type y path (atributos de MeshTexture)
    const std::string& get_type() const noexcept { return type; }
    const std::string& get_path() const noexcept { return path; }
    void set_type(const std::string& t) noexcept { type = t; }
    void set_path(const std::string& p) noexcept { path = p; }
    
private:
    void clear() noexcept;
    
    GLuint texture_id{0};
    int width{0};
    int height{0};
    int channels{0};
    std::string type; // "texture_albedo", "texture_normal", "texture_metallic", "texture_roughness", "texture_ao", "texture_diffuse"
    std::string path; // Ruta del archivo de textura
};

#endif // TEXTURE_HPP 