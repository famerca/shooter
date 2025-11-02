#include <DirectionalLight.hpp>

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity,
                                   GLfloat x_dir, GLfloat y_dir, GLfloat z_dir) noexcept
    : Light(red, green, blue, intensity)
{
    direction = glm::vec3(x_dir, y_dir, z_dir);
    changed = true;
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color, GLfloat intensity)
    : Light(color, intensity)
{
    this->direction = direction;
    changed = true;
}


void DirectionalLight::setDirection(glm::vec3 direction)
{
    this->direction = direction;
    changed = true;
}

bool DirectionalLight::isChanged() const noexcept
{
    return changed;
}

std::tuple<GLfloat, glm::vec3, glm::vec3> DirectionalLight::renderData() noexcept
{
    changed = false;
    return std::make_tuple(intensity, color, direction);
}

