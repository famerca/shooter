#include "Light.hpp"
#include "Shader.hpp"

#ifndef DIRECTIONAL_LIGHT_HPP
#define DIRECTIONAL_LIGHT_HPP

class DirectionalLight : Light
{

public:
    DirectionalLight() = default;

    DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity,
                     GLfloat x_dir, GLfloat y_dir, GLfloat z_dir) noexcept;

    void render() const noexcept override;
    void bindShader(DirectionalLightUniforms) noexcept;

protected:

    glm::vec3 direction{0.f, -1.f, 0.f};
    GLuint direction_id{0};
};

#endif // DIRECTIONAL_LIGHT_HPP