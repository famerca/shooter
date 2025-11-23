#include <GL/glew.h>
#include <glm/glm.hpp>
#include <tuple>

#ifndef LIGHT_HPP
#define LIGHT_HPP
namespace Engine
{

class Light
{
protected:
    /* data */
    glm::vec3 color{1.f, 1.f, 1.f};
    GLfloat intensity{1.f};
public:
     Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity);
     Light(glm::vec3 color, GLfloat intensity);
     void setColor(glm::vec3);
     void setColor(GLfloat, GLfloat, GLfloat);
     void setIntensity(GLfloat);
     
    ~Light();

};

}

#endif