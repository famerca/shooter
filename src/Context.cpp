#include "Context.hpp"

Context::Context()
{
    projection = glm::perspective(45.f, 800.f / 600.f, 0.1f, 100.f);
}

Context::Context(const GLfloat& aspect_ratio)
{
    projection = glm::perspective(45.f, aspect_ratio, 0.1f, 100.f);
}

Context::~Context()
{

}

std::shared_ptr<Context> Context::create(const float& aspect_ratio)
{
    return std::make_shared<Context>(aspect_ratio);
}

void Context::addShader(std::string vertex_shader_name, std::string fragment_shader_name)
{
    fs::path vertex_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / vertex_shader_name;
    fs::path fragment_shader_path =  fs::path{__FILE__}.parent_path().parent_path() / "shaders" / fragment_shader_name;

    if(!std::filesystem::exists(vertex_shader_path) || !std::filesystem::exists(fragment_shader_path))
    {
        throw std::runtime_error("Shader not found");
    }

    shader_list.push_back(Shader::create_from_files(vertex_shader_path, fragment_shader_path));
}

std::shared_ptr<Shader> Context::get_shader_list(unsigned int index = 0) const noexcept
{
    if(index >= shader_list.size())
    {
        throw std::runtime_error("Index out of range");
    }

    return shader_list[index];
}