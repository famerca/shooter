#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef LIGHT_HPP
#define LIGHT_HPP

class Light
{
protected:
    /* data */
    glm::vec3 color{1.f, 1.f, 1.f};
    GLfloat intensity{1.f};
    GLint color_id{0};
    GLint intensity_id{0};
public:
     Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity);
     Light(glm::vec3 color, GLfloat intensity);
    ~Light();
    void setIntensityID(GLint idGLuint) noexcept { intensity_id = idGLuint; }
    void setColorID(GLint idGLuint) noexcept { color_id = idGLuint; }
    virtual void render() const noexcept;


};

#endif