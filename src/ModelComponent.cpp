#include "ModelComponent.hpp"
#include "Utils.hpp"
ModelComponent::ModelComponent( std::shared_ptr<GameObject> _owner) : Component(_owner, Component::Type::Model)
{
    model = nullptr;
    sp_shader = "";
    r_model = glm::mat4(1.f);
    relative = false;
}

ModelComponent::~ModelComponent()
{
    owner = nullptr;
}

void ModelComponent::update(const GLfloat &dt)
{
    //model->render();
}

const glm::mat4& ModelComponent::getModelMatrix()
{
    return r_model;
}

void ModelComponent::setRelativeModel(
    glm::vec3 position, 
    float angle, 
    glm::vec3 axis, 
    glm::vec3 scale
)
{
    r_model = glm::mat4(1.f);

    r_model = glm::translate(r_model, position);

    r_model = glm::rotate(r_model, glm::radians(angle), axis);

    r_model = glm::scale(r_model, scale);

    Utils::printMat4(r_model);
    
    relative = true;

}

bool ModelComponent::loadModel(const std::string& model_name, const float& scale_factor)
{
    try
    {
        /* code */
        model = std::make_shared<Model>(model_name, scale_factor);
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

