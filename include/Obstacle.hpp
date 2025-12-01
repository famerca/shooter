#ifndef OBSTACLE_H
#define OBSTACLE_H
#include <memory>
#include <string>
#include <variant>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <GLS/Scene.hpp>
#include <GLS/GameObject.hpp>
#include <GLS/Listener.hpp>

#include "inputManager.hpp"

class Level;

using scriptParams = std::vector<
std::variant< std::string, float, int, bool*, inputManager*, Engine::GameObject*>
>;

struct ObstacleSettings
{
    JPH::Vec3 box_shape = JPH::Vec3::sZero();
    glm::vec3 rel_pos = {0.f, 0.f, 0.f};
    glm::vec3 rel_scale = {1.f, 1.f, 1.f};
    glm::vec3 rel_axis = {0.f, 1.f, 0.f};
    float rel_angle = 0.f;
    Engine::BodyType body_type = Engine::BodyType::Dynamic;
    Engine::Listener::Callback onContactStart = nullptr;
    Engine::Listener::Callback onContactEnd = nullptr;
    unsigned user_index{0};
    glm::vec3 scale = {1.f, 1.f, 1.f};
    glm::vec3 axis = {0.f, 1.f, 0.f};
    float angle = 0.f;
    std::string script = "";
    scriptParams script_params;
};

class Obstacle: public std::enable_shared_from_this<Obstacle>
{
    private:
        friend class Level;
        std::string filename;
        std::string tag;
        unsigned m_index;
        std::shared_ptr<Engine::Scene> m_scene;
        std::shared_ptr<Engine::GameObject> m_object;

        void initCollitions(const ObstacleSettings& settings, const std::shared_ptr<Engine::Scene>& scene);

    public:

    Obstacle(
        const std::shared_ptr<Engine::Scene>& scene,
        const std::string& filename,
        const std::string& tag,
        const glm::vec3& pos
    );

    Obstacle(
        const std::shared_ptr<Engine::Scene>& scene,
        const std::string& filename,
        const std::string& tag,
        const glm::vec3& pos,
        ObstacleSettings settings
    );


    Obstacle(
        const std::shared_ptr<Engine::Scene>& scene,
        unsigned index_ref,
        const std::string& tag,
        const glm::vec3& pos,
        ObstacleSettings settings
    );

    

};


#endif // OBJECT_H