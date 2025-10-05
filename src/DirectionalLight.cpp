#include <DirectionalLight.hpp>

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity,
                                   GLfloat x_dir, GLfloat y_dir, GLfloat z_dir) noexcept
    : Light(red, green, blue, intensity)
{
    direction = glm::vec3(x_dir, y_dir, z_dir);
}

void DirectionalLight::render() const noexcept
{
    Light::render();
    glUniform3f(direction_id, direction.x, direction.y, direction.z);
  
}

void DirectionalLight::bindShader(DirectionalLightUniforms uniforms) noexcept
{
    Light::setColorID(uniforms.color_id);
    Light::setIntensityID(uniforms.diffuse_intensity_id);
    this->direction_id = uniforms.direction_id;
}