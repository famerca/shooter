#include "Component.hpp"
#include "Model.hpp"

#ifndef MODEL_COMPONENT_HPP
#define MODEL_COMPONENT_HPP


class ModelComponent : public Component
{
private:
    std::shared_ptr<Model> model;
    std::string sp_shader;
public:
    ModelComponent(Owner);
    ~ModelComponent();

    void update(GLfloat);

    bool loadModel(std::string);
    
    void start();
    std::shared_ptr<Model> getModel()
    {
        return model;
    }
};

#endif