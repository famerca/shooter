#include "Model.hpp"
#include "Utils.hpp"

Model::Model(const std::string &model_name)
{

    std::filesystem::path model_path{std::filesystem::path{__FILE__}.parent_path().parent_path() / "models" / model_name};

    if (!std::filesystem::exists(model_path))
    {
        throw std::runtime_error("Model " + model_name + " not found");
    }

    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(model_path.string(),
                                             aiProcess_Triangulate |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("Error de Assimp al importar el modelo: " + std::string(importer.GetErrorString()));
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    //     aiMesh* mesh = scene->mMeshes[i];
    //     // Procesar los vértices, normales y coordenadas de textura
    //     // Aquí es donde copias los datos a VBO y VAO de OpenGL.
    // }

    if (scene->mNumMeshes != 1)
    {
        throw std::runtime_error("El modelo debe tener un solo objeto");
    }

    process_mesh(scene->mMeshes[0]);
}

void Model::process_mesh(const aiMesh *mesh)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Recorre los datos del aiMesh y los copia
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // Copia las posiciones
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        // Copia las normales
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        // Copia las coordenadas de textura
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        vertices.push_back(vertex);
    }

    // Recorre las caras (faces) para obtener los índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    this->mesh = Mesh::create(vertices, indices);
}

void Model::render() noexcept
{
    this->transforms();

    auto shader = context->get_shader_list(0);

    shader->use();

    glUniformMatrix4fv(shader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(context->get_projection()));
    glUniformMatrix4fv(shader->get_uniform_model_id(), 1, GL_FALSE, glm::value_ptr(model_matrix));
    
    mesh->render();

}

void Model::clear() noexcept
{
}

void Model::transforms() noexcept
{
    model_matrix = glm::mat4(1.f);

    model_matrix = glm::translate(model_matrix, m_translate);
    model_matrix = glm::rotate(model_matrix, m_rotate_angle, m_rotate_axis);
    model_matrix = glm::scale(model_matrix, m_scale);
}

void Model::rotate(glm::vec3 ax, float angle) noexcept
{
    m_rotate_angle = Utils::to_radian(angle);
    m_rotate_axis = ax;
}

void Model::rotate(Model::AXIS ax, float angle) noexcept
{
    if(ax == Model::X)
    {
        m_rotate_angle = Utils::to_radian(angle);
        m_rotate_axis = glm::vec3(1.f, 0.f, 0.f);
    }

    else if(ax == Model::Y)
    {
        m_rotate_angle = Utils::to_radian(angle);
        m_rotate_axis = glm::vec3(0.f, 1.f, 0.f);
    }

    else if(ax == Model::Z)
    {
        m_rotate_angle = Utils::to_radian(angle);
        m_rotate_axis = glm::vec3(0.f, 0.f, 1.f);
    }
    
}

void Model::translate(float x = 0.f, float y = 0.f, float z = 0.f) noexcept
{
    m_translate = glm::vec3(x, y, z);
  
}

void Model::scale(float x = 0.f, float y = 0.f, float z = 0.f) noexcept
{
    m_scale = glm::vec3(x, y, z);
}
void Model::setContext(std::shared_ptr<Context> context) noexcept
{
    this->context = context;
}