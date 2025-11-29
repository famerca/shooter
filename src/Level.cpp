#include <iostream>
#include <glm/gtx/io.hpp>

#include <GLS/GameObject.hpp>
#include <GLS/TransformComponent.hpp>
#include <GLS/Physics.hpp>

#include "Level.hpp"


Level::Level(std::shared_ptr<Engine::Scene> scene, unsigned user_index): scene(scene), user_index(user_index)
{

}

Level::~Level()
{

}

void Level::init(const init_list& list)
{
    for (auto& obs : list)
    {
        auto it = tags_map.find(obs.tag);
        if(it == tags_map.end())
        {
            auto obstacle = std::make_shared<Obstacle>(scene, obs.filename, obs.tag, obs.pos, obs.settings);
            obstacles.push_back(obstacle);
            tags_map.insert({obs.tag, obstacle->m_index});
        }else{
            auto obstacle = std::make_shared<Obstacle>(scene, it->second, obs.tag, obs.pos, obs.settings);
            obstacles.push_back(obstacle);
        }
    }

    scene->at(user_index)->getBody()->constraintRotation(obstacles[0]->m_object->getBody());
}


