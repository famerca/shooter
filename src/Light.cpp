#include <Light.hpp>

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity)
{
    color = glm::vec3(red, green, blue);
    intensity = intensity;
}

Light::~Light()
{
}

void Light::render() const noexcept
{
    glUniform3f(color_id, color.r, color.g, color.b);
    glUniform1f(intensity_id, intensity);
}

