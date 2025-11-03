#pragma once

#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef SKYBOX_HPP
#define SKYBOX_HPP

class Skybox
{
public:
    Skybox() = default;

    Skybox(const Skybox& skybox) = delete;

    Skybox(Skybox&& skybox) = delete;

    ~Skybox();

    Skybox& operator=(const Skybox& skybox) = delete;

    Skybox& operator=(Skybox&& skybox) = delete;

    static std::shared_ptr<Skybox> create() noexcept;

    bool load_cubemap(const std::vector<std::filesystem::path>& faces) noexcept;

    void render() const noexcept;

    GLuint get_cubemap_texture() const noexcept { return cubemap_texture; }

    bool has_texture() const noexcept { return cubemap_texture != 0; }

private:
    void clear() noexcept;
    void create_cube_mesh() noexcept;

    GLuint VAO_id{0};
    GLuint VBO_id{0};
    GLuint cubemap_texture{0};
    GLsizei vertex_count{0};
};

#endif

