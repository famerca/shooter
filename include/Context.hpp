#include <memory>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.hpp>

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

namespace fs = std::filesystem;

class Context
{
public:
    Context();

    Context(const GLfloat& aspect_ratio);

    Context(const Context& context) = delete;

    Context(Context&& context) = delete;

    ~Context();

    static std::shared_ptr<Context> create(const float& aspect_ratio);

    void addShader(std::string vertex_shader_path, std::string fragment_shader_path);

    glm::mat4 get_projection() const noexcept { return projection; }

    std::shared_ptr<Shader> get_shader_list(unsigned int index) const noexcept;
    
    private:

    std::vector<std::shared_ptr<Shader>> shader_list;
    glm::mat4 projection;

};

#endif // CONTEXT_HPP

