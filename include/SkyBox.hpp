#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Mesh.hpp>

namespace Engine
{

class SkyBox
{
public:
    SkyBox() = default;

    SkyBox(const std::string& dir, const std::vector<std::filesystem::path>& face_filenames) noexcept;

    void set_view(const glm::mat4& view) noexcept;

    const glm::mat4& get_view() const noexcept;

    ~SkyBox();

    void render() const noexcept;

private:
    std::shared_ptr<Mesh> mesh{nullptr};
    glm::mat4 skybox_view{glm::mat4(1.f)};
   
    
    GLuint texture_id{0};
};

}