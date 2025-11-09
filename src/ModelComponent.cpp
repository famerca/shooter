#include "ModelComponent.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

ModelComponent::ModelComponent( std::shared_ptr<GameObject> _owner) : Component(_owner, Component::Type::Model)
{
    model = nullptr;
    sp_shader = "";
}

ModelComponent::~ModelComponent()
{
    owner = nullptr;
}

void ModelComponent::update(const GLfloat &dt)
{
    //model->render();
}

bool ModelComponent::loadModel(const std::string& model_name)
{
    try
    {
        // Construir la ruta del modelo
        std::filesystem::path model_path{std::filesystem::path{__FILE__}.parent_path().parent_path() / "models" / model_name};
        
        // Verificar que el archivo existe
        if (!std::filesystem::exists(model_path))
        {
            std::cerr << "Error: Modelo no encontrado: " << model_path << std::endl;
            return false;
        }

        // Importar el modelo con Assimp
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(model_path.string(),
                                                 aiProcess_Triangulate |
                                                     aiProcess_FlipUVs |
                                                     aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "Error de Assimp al importar el modelo: " << std::string(importer.GetErrorString()) << std::endl;
            return false;
        }
        
        // Crear el modelo vacío y procesar la escena
        model = std::make_shared<Model>();
        model->processScene(scene, model_path);
        return true;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error al cargar modelo: " << e.what() << std::endl;
        return false;
    }
}

void ModelComponent::start()
{
    
}

