#include "ModelComponent.hpp"

ModelComponent::ModelComponent( std::shared_ptr<GameObject> _owner) : Component(_owner, Component::Type::Model)
{
    model = nullptr;
    sp_shader = "";
}

ModelComponent::~ModelComponent()
{
    owner = nullptr;
}

void ModelComponent::update(GLfloat)
{
    //model->render();
}

bool ModelComponent::loadModel(std::string model_name)
{
    try
    {
        /* code */
        model = std::make_shared<Model>(model_name);
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

