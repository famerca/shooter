#include "Component.hpp"
#include "Model.hpp"

#ifndef MODEL_COMPONENT_HPP
#define MODEL_COMPONENT_HPP

namespace Engine
{

class ModelComponent : public Component
{
private:
    std::shared_ptr<Model> model;
    //Modelo relativo al gameobject
    glm::mat4 r_model;
    bool relative;
    std::string sp_shader;

public:
    ModelComponent(Owner);
    ~ModelComponent();


    void setRelativeModel(
        glm::vec3 position = {0.f, 0.f, 0.f}, 
        float angle = 0.f, 
        glm::vec3 axis = {0.f, 1.f, 0.f},
        glm::vec3 scale = {1.f, 1.f, 1.f}
    );

    const glm::mat4& getModelMatrix();

    const bool &isRelative() const noexcept
    {
        return relative;
    }

    void update(const GLfloat &dt);

    bool loadModel(const std::string& model_name,const float& scale_factor = 1.0f);
    
    void start();
    std::shared_ptr<Model> getModel()
    {
        return model;
    }
};

}
#endif