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

    void rotate(glm::vec3 ax, float angle) noexcept;

    void rotate(Model::AXIS ax, float angle) noexcept;

    void translate(float x, float y, float z) noexcept;

    void scale(float x, float y, float z) noexcept;
    
    void setContext(std::shared_ptr<Context> context) noexcept;
    
    void load_textures(const aiScene* scene, const std::filesystem::path& model_path) noexcept;
    void create_default_texture() noexcept;
    std::shared_ptr<Texture> create_texture_from_embedded_data(const aiTexture* embedded_texture) noexcept;
    std::shared_ptr<Texture> create_texture_from_uncompressed_data(const aiTexture* embedded_texture) noexcept;

private:
    void clear() noexcept;
    void process_mesh(const aiMesh *mesh, const std::string& model_name);
    void transforms() noexcept;

    glm::mat4 model_matrix;
    glm::vec3 m_translate;
    glm::vec3 m_rotate_axis;
    float m_rotate_angle;
    glm::vec3 m_scale;

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Context> context;
    std::vector<std::shared_ptr<Texture>> textures;
};

#endif // MODEL_HPP
