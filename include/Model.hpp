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

    void render() noexcept;
    
    void load_textures(const aiScene* scene, const std::filesystem::path& model_path) noexcept;
    void create_default_texture() noexcept;
    
    std::shared_ptr<Texture> create_texture_from_embedded_data(const aiTexture* embedded_texture) noexcept;
    std::shared_ptr<Texture> create_texture_from_uncompressed_data(const aiTexture* embedded_texture) noexcept;
    
    std::shared_ptr<Mesh> getMesh() const noexcept { return mesh; }
    std::vector<std::shared_ptr<Texture>> getTextures() const noexcept { return textures; }
    
private:
    void clear() noexcept;
    void process_mesh(const aiMesh *mesh, const std::string& model_name);

    std::shared_ptr<Mesh> mesh;
    std::vector<std::shared_ptr<Texture>> textures;
};

#endif // MODEL_HPP
