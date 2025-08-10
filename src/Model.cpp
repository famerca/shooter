#include "Model.hpp"

Model::Model(const std::string& model_name)
{
    std::filesystem::path model_path{std::filesystem::path{__FILE__}.parent_path().parent_path() / "models" / model_name};

    if(!std::filesystem::exists(model_path))
    {
        throw std::runtime_error("Model " + model_name + " not found");
    }

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(model_path.string(),
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

    if(scene->mNumMeshes != 1)
    {
        throw std::runtime_error("El modelo debe tener un solo objeto");
    }

    process_mesh(scene->mMeshes[0]);

}

void Model::process_mesh(const aiMesh* mesh)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Recorre los datos del aiMesh y los copia
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // Copia las posiciones
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        // Copia las normales
        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        // Copia las coordenadas de textura
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        vertices.push_back(vertex);
    }

    // Recorre las caras (faces) para obtener los índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    this->mesh = Mesh::create(vertices, indices);

}

void Model::render() const noexcept
{
    mesh->render();
}

void Model::clear() noexcept
{
    
}