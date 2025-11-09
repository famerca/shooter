#include <filesystem>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"
#include "Context.hpp"
#include "Texture.hpp"

#ifndef MODEL_HPP
#define MODEL_HPP

class Model
{
friend class ModelComponent;

public:
    enum AXIS
    {
        X,
        Y,
        Z
    };

    Model() = default;

    Model(const std::string &model_name);

    Model(const Model &model) = delete;

    Model(Model &&model) = delete;

    ~Model() = default;

    Model &operator=(const Model &model) = delete;

    Model &operator=(Model &&model) = delete;

    void render(GLuint texture_id) noexcept;
    
    void load_textures(const aiScene* scene, const std::filesystem::path& model_path) noexcept;
    void create_default_texture() noexcept;
    
    std::shared_ptr<Texture> create_texture_from_embedded_data(const aiTexture* embedded_texture) noexcept;
    std::shared_ptr<Texture> create_texture_from_uncompressed_data(const aiTexture* embedded_texture) noexcept;
    
    std::shared_ptr<Mesh> getMesh() const noexcept { return meshes.empty() ? nullptr : meshes[0]; } // Compatibilidad con código existente
    std::vector<std::shared_ptr<Mesh>> getMeshes() const noexcept { return meshes; }
    std::vector<std::shared_ptr<Texture>> getTextures() const noexcept { return textures; }
    
private:
    void clear() noexcept;
    void loadModel(const std::filesystem::path& path);
    void processNode(aiNode* node, const aiScene* scene, const std::filesystem::path& model_path);
    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, const std::filesystem::path& model_path);
    std::vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::filesystem::path& model_path, const aiScene* scene);
    
    // Método legacy mantenido para compatibilidad
    void process_mesh(const aiMesh *mesh, const aiScene* scene, const std::filesystem::path& model_path);

    std::vector<std::shared_ptr<Mesh>> meshes; // Cambiado a vector para soportar múltiples meshes
    std::vector<std::shared_ptr<Texture>> textures;
};

#endif // MODEL_HPP
