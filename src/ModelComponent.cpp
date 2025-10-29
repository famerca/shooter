#include "ModelComponent.hpp"

ModelComponent::ModelComponent(GameObject *_owner)
{
    owner = _owner;
    model = nullptr;
    sp_shader = '';
}

ModelComponent::~ModelComponent()
{
    owner = nullptr;
}

void ModelComponent::update(GLfloat)
{
    model->render();
}

bool ModelComponent::loadModel(std::string model_name)
{
    try
    {
        /* code */
        model = new Model(model_name);
        return true;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
}

void ModelComponent::start()
{
    
}

