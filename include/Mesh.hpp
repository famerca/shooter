#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef MESH_HPP
#define MESH_HPP

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

    Mesh(const Mesh& mesh) = delete;

    Mesh(Mesh&& mesh) = delete;

    ~Mesh();

    Mesh& operator = (const Mesh& mesh) = delete;

    Mesh& operator = (Mesh&& mesh) = delete;

    void render() const noexcept;
    
    const std::string& get_name() const noexcept { return name; }
    void set_name(const std::string& n) noexcept { name = n; }
    
private:
    void clear() noexcept;

    GLuint VAO_id{0};
    GLuint VBO_id{0};
    GLuint IBO_id{0};
    GLsizei index_count{0};
    std::string name;
};

#endif // MESH_HPP