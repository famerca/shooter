#pragma once

#include <memory>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef MESH_HPP
#define MESH_HPP

class Texture;
class Shader;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct MeshTexture {
    std::shared_ptr<Texture> texture;
    std::string type; // "texture_albedo", "texture_normal", "texture_metallic", "texture_roughness", "texture_ao"
    std::string path;
};

class Mesh
{
public:
    Mesh() = default;

    static std::shared_ptr<Mesh> create(const std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) noexcept;
    static std::shared_ptr<Mesh> create(const std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const std::vector<MeshTexture>& textures) noexcept;

    Mesh(const Mesh& mesh) = delete;

    Mesh(Mesh&& mesh) = delete;

    ~Mesh();

    Mesh& operator = (const Mesh& mesh) = delete;

    Mesh& operator = (Mesh&& mesh) = delete;

    void render() const noexcept;
    void render(std::shared_ptr<class Shader> shader) const noexcept; // Para renderizado PBR con múltiples texturas
    
    const std::string& get_name() const noexcept { return name; }
    void set_name(const std::string& n) noexcept { name = n; }
    
    const std::vector<MeshTexture>& get_textures() const noexcept { return textures; }
    void set_textures(const std::vector<MeshTexture>& tex) noexcept { textures = tex; }
    
private:
    void clear() noexcept;
    
    // Métodos privados para manejo de texturas PBR (nuevo código)
    struct DefaultTextures {
        GLuint white{0};
        GLuint normal{0};
        GLuint black{0};
    };
    
    static DefaultTextures& getDefaultTextures() noexcept;
    static void initializeDefaultTextures(DefaultTextures& defaults) noexcept;
    void bindPBRTextures(std::shared_ptr<Shader> shader, unsigned int& textureSlot, bool& hasAlbedo, bool& hasNormal, bool& hasMetallic, bool& hasRoughness, bool& hasAO) const noexcept;
    void bindDefaultTextures(std::shared_ptr<Shader> shader, unsigned int& textureSlot, bool hasAlbedo, bool hasMetallic, bool hasRoughness, bool hasAO, const DefaultTextures& defaults) const noexcept;

    GLuint VAO_id{0};
    GLuint VBO_id{0};
    GLuint IBO_id{0};
    GLsizei index_count{0};
    std::string name;
    std::vector<MeshTexture> textures;
};

#endif // MESH_HPP