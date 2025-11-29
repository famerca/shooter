#include <GLS/Physics.hpp>
#include <GLS/Listener.hpp>

#include "Obstacle.hpp"
#include "Scripts.hpp"


Obstacle::Obstacle(
    const std::shared_ptr<Engine::Scene>& scene,
    const std::string& filename,
    const std::string& tag,
    const glm::vec3& pos
): filename(filename), tag(tag)
{
    m_index = scene->createGameObject();
    m_object = scene->at(m_index);
    m_scene = scene;

    m_object->getTransform()->translate(pos);
    m_object->getTransform()->scale({1.f, 1.f, 1.f});
    m_object->getTransform()->rotate(0.f, {0.f, 1.f, 0.f});
}


Obstacle::Obstacle(
    const std::shared_ptr<Engine::Scene>& scene,
    const std::string& filename,
    const std::string& tag,
    const glm::vec3& pos,
    ObstacleSettings settings
): filename(filename), tag(tag)
{
    m_index = scene->createGameObject();
    m_object = scene->at(m_index);
    m_scene = scene;

    m_object->getTransform()->translate(pos);
    m_object->getTransform()->scale(settings.scale);
    m_object->getTransform()->rotate(settings.angle, settings.axis);

    

    if(settings.rel_pos != glm::vec3(0.f, 0.f, 0.f))
    {
        std::shared_ptr<Engine::ModelComponent>  m = scene->createModel(m_index);
        m->loadModel(filename);
        m->setRelativeModel(
            settings.rel_pos,
            settings.rel_angle,
            settings.rel_axis,
            settings.rel_scale
        );
    }else
        scene->createModel(m_index)->loadModel(filename);
        

    if(settings.box_shape != JPH::Vec3::sZero())
    {
        m_object->setBody(Engine::Physics::Get().CreateBox(settings.box_shape, JPH::Vec3::sZero(), settings.body_type));
    }

    initCollitions(settings, m_scene);
}

Obstacle::Obstacle(
    const std::shared_ptr<Engine::Scene>& scene,
    unsigned index_ref,
    const std::string& tag,
    const glm::vec3& pos,
    ObstacleSettings settings
): tag(tag)

{
    m_index = scene->cloneGameObject(index_ref);
    m_object = scene->at(m_index);
    m_scene = scene;

    m_object->getTransform()->translate(pos);
    m_object->getTransform()->scale(settings.scale);
    m_object->getTransform()->rotate(settings.angle, settings.axis);

    auto body = m_object->getBody();

    if(body)
    {
        body->SetPosition({pos.x, pos.y, pos.z});
    }

    initCollitions(settings, m_scene);

}


void Obstacle::initCollitions(const ObstacleSettings& settings, const std::shared_ptr<Engine::Scene>& scene)
{
    if( m_object->getBody() == nullptr)
        return;


    if(settings.script == "PlataformaMovil")
    {
        m_object->addScript(std::make_shared<PlataformaMovil>());
    }

    if(settings.onContactStart)
    {
        Engine::Listener::Get().Add(scene, Engine::Listener::Event::ContactAdded, m_index, settings.user_index, settings.onContactStart);
    }

    if(settings.onContactEnd)
    {
        Engine::Listener::Get().Add(scene, Engine::Listener::Event::ContactRemoved, m_index, settings.user_index, settings.onContactEnd);
    }
}