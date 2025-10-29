#include "Component.hpp"
#include "Model.hpp"

#ifndef MODEL_COMPONENT_HPP
#define MODEL_COMPONENT_HPP


class ModelComponent : Component
{
private:
    Model *model;
    std::string sp_shader;
public:
    ModelComponent(GameObject *);
    ~ModelComponent();

    void update(GLfloat);

    bool loadModel(std::string);
    
    void start();
    GameObject* getOwner(){
        return owner;
    }
};

#endif