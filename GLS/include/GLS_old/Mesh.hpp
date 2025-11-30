#pragma once

#include <memory>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef MESH_HPP
#define MESH_HPP

namespace Engine
{

class Texture;
class Shader;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh
{
public:
    Mesh() = default;

    static std::shared_ptr<Mesh> create(const std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) noexcept;
    static std::shared_ptr<Mesh> create(const std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const std::vector<std::shared_ptr<Texture>>& textures) noexcept;
    Mesh(const Mesh& mesh) = delete;

    Mesh(Mesh&& mesh) = delete;

    ~Mesh();

    Mesh& operator = (const Mesh& mesh) = delete;

    Mesh& operator = (Mesh&& mesh) = delete;

    void render() const noexcept;
    
    const std::string& get_name() const noexcept { return name; }
    void set_name(const std::string& n) noexcept { name = n; }

    const std::vector<std::shared_ptr<Texture>>& get_textures() const noexcept { return textures; }
    void set_textures(const std::vector<std::shared_ptr<Texture>>& tex) noexcept { textures = tex; }
    
private:
    void clear() noexcept;

    struct DefaultTextures {
        GLuint albedo{0};      
        GLuint normal{0};    
        GLuint metallic{0};   
        GLuint roughness{0};   
        GLuint ao{0};          
    };

    GLuint VAO_id{0};
    GLuint VBO_id{0};
    GLuint IBO_id{0};
    GLsizei index_count{0};
    std::string name;
    std::vector<std::shared_ptr<Texture>> textures;
};

}
#endif // MESH_HPP