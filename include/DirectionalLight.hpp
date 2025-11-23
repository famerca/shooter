#include "Light.hpp"
#include "Shader.hpp"

#ifndef DIRECTIONAL_LIGHT_HPP
#define DIRECTIONAL_LIGHT_HPP

namespace Engine
{

class DirectionalLight : Light
{

public:
    DirectionalLight() = default;

    DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity,
                     GLfloat x_dir, GLfloat y_dir, GLfloat z_dir) noexcept;

    DirectionalLight(glm::vec3 direction, glm::vec3 color, GLfloat intensity);

    void setDirection(glm::vec3);
 
    bool isChanged() const noexcept;

    /**
     * @brief Render data <Intensity, Color, Direction>
     * @return std::tuple<GLfloat, glm::vec3, glm::vec3>
     */

    std::tuple<GLfloat, glm::vec3, glm::vec3> renderData() noexcept;

private:

    glm::vec3 direction{0.f, -1.f, 0.f};
    bool changed;
};

}

#endif // DIRECTIONAL_LIGHT_HPP