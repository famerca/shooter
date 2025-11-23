#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include <GL/glew.h>


#ifndef SHADER_HPP
#define SHADER_HPP

namespace Engine
{

struct DirectionalLightUniforms
{
    GLuint color_id;
    GLuint diffuse_intensity_id;
    GLuint direction_id;
};

class Shader
{
public:

    enum class LIST{
        BASE = 0,
        SKYBOX = 1
    };
    
    Shader() = default;

    Shader(const Shader& shader) = delete;

    Shader(Shader&& shader) = delete;

    ~Shader();

    Shader& operator = (const Shader& shader) = delete;

    Shader& operator = (Shader&& shader) = delete;

    static std::shared_ptr<Shader> create_from_strings(std::string_view vertex_shader_code, std::string_view fragment_shader_code) noexcept;

    static std::shared_ptr<Shader> create_from_files(std::filesystem::path vertex_shader_path, std::filesystem::path fragment_shader_path) noexcept;

    GLuint get_uniform_projection_id() const noexcept { return uniform_projection_id; }

    GLuint get_uniform_model_id() const noexcept { return uniform_model_id; }
    
    GLuint get_uniform_diffuse_texture_id() const noexcept { return uniform_diffuse_texture_id; }

    GLuint get_uniform_view_id() const noexcept { return uniform_view_id; }

    GLuint get_uniform_line_color_id() const noexcept { return uniform_line_color_id; }

    void use() const noexcept;

    DirectionalLightUniforms get_uniform_directional_light() const noexcept { return uniform_directional_light; }

   

private:
    void clear() noexcept;

    void create_program(std::string_view vertex_shader_code, std::string_view fragment_shader_code) noexcept;

    void create_shader(std::string_view shader_code, GLenum shader_type) noexcept;

    static std::string read_file(const std::filesystem::path& shader_path) noexcept;

    GLuint program_id{0};
    GLuint uniform_projection_id{0};
    GLuint uniform_view_id{0};
    GLuint uniform_model_id{0};
    GLuint uniform_diffuse_texture_id{0};
    GLuint uniform_line_color_id{0};

    DirectionalLightUniforms uniform_directional_light;
};

}
#endif // SHADER_HPP