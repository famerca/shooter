#include <Light.hpp>

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity)
{
    color = glm::vec3(red, green, blue);
    intensity = intensity;
}

Light::Light(glm::vec3 color, GLfloat intensity)
{
    this->color = color;
    this->intensity = intensity;
}

Light::~Light()
{
}

void Light::setColor(glm::vec3 color)
{
    this->color = color;
}

void Light::setColor(GLfloat red, GLfloat green, GLfloat blue)
{
    this->color = glm::vec3(red, green, blue);
}

void Light::setIntensity(GLfloat intensity)
{
    this->intensity = intensity;
}



