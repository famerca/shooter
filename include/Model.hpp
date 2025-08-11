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

    

private:
    void clear() noexcept;
    void process_mesh(const aiMesh *mesh);
    void transforms() noexcept;

    glm::mat4 model_matrix;
    glm::vec3 m_translate;
    glm::vec3 m_rotate_axis;
    float m_rotate_angle;
    glm::vec3 m_scale;

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Context> context;
};

#endif // MODEL_HPP
