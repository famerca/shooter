#ifndef OBSTACLE_H
#define OBSTACLE_H
#include <memory>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <GLS/Scene.hpp>
#include <GLS/GameObject.hpp>
#include <GLS/Listener.hpp>

class Level;

struct ObstacleSettings
{
    JPH::Vec3 box_shape = JPH::Vec3::sZero();
    JPH::Vec3 rel_pos = JPH::Vec3::sZero();
    Engine::BodyType body_type = Engine::BodyType::Dynamic;
    JPH::Vec3 rel_scale = JPH::Vec3::sZero();
    const Engine::Listener::Callback &onContactStart = nullptr;
    const Engine::Listener::Callback &onContactEnd = nullptr;
    glm::vec3 scale = {1.f, 1.f, 1.f};
    glm::vec3 axis = {0.f, 1.f, 0.f};
    float angle = 0.f;
};

class Obstacle: public std::enable_shared_from_this<Obstacle>
{

    Obstacle(
        const std::shared_ptr<Engine::Scene> scene,
        const std::string& filename,
        const std::string& type,
        const glm::vec3& pos
    ): filename(filename), type(type), pos(pos), scale(1.f,1.f,1.f), axis(0.f, 1.f, 0.f), angle(0.f)
    {
        index = scene->createGameObject();
        m_object = scene->at(index);
        m_scene = scene;

        m_object->getTransform()->translate(pos);
        m_object->getTransform()->scale(scale);
        m_object->getTransform()->rotate(angle, axis);
    }

        Obstacle(
        const std::shared_ptr<Engine::Scene> scene,
        const std::string& filename,
        const std::string& type,
        const glm::vec3& pos,
        ObstacleSettings settings

    ): filename(filename), type(type), pos(pos), scale(1.f,1.f,1.f), axis(0.f, 1.f, 0.f), angle(0.f)
    {
        index = scene->createGameObject();
        m_object = scene->at(index);
        m_scene = scene;

        m_object->getTransform()->translate(pos);
        m_object->getTransform()->scale(scale);
        m_object->getTransform()->rotate(angle, axis);
    }





    private:
        friend class Level;
        std::string filename;
        std::string type;
        glm::vec3 pos;
        glm::vec3 scale;
        glm::vec3 axis;
        float angle;
        unsigned index;
        std::shared_ptr<Engine::Scene> m_scene;
        std::shared_ptr<Engine::GameObject> m_object;

};


#endif // OBJECT_H