#include <filesystem>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "Mesh.hpp"

#ifndef MODEL_HPP
#define MODEL_HPP

class Model
{
public:
    Model() = default;

    Model(const std::string& model_name);

    Model(const Model& model) = delete;

    Model(Model&& model) = delete;

    ~Model() = default;

    Model& operator = (const Model& model) = delete;

    Model& operator = (Model&& model) = delete;

    void render() const noexcept;

private:
    void clear() noexcept;
    void process_mesh(const aiMesh* mesh);


    std::shared_ptr<Mesh> mesh;
};

#endif // MODEL_HPP