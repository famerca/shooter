#include <iostream>
#include <glm/gtx/io.hpp>

#include <GLS/GameObject.hpp>
#include <GLS/TransformComponent.hpp>
#include <GLS/Physics.hpp>

#include "Level.hpp"


Level::Level(const init_list& list): obstacles(list)
{

}

Level::~Level()
{

}

void Level::init(std::shared_ptr<Engine::Scene> scene)
{
    for (auto& obstacle : obstacles)
    {
        std::cout << "Obstacle: " << obstacle.filename << std::endl;
        auto obs = scene->createGameObject();
        scene->createModel(obs)->loadModel(obstacle.filename);
        std::cout << "Trasladado: " << obstacle.pos << std::endl;
        scene->at(obs)->getTransform()->translate(obstacle.pos);
        std::cout << "Escalado: " << obstacle.scale << std::endl;
        scene->at(obs)->getTransform()->scale(obstacle.scale);
        std::cout << "Modelo cargado: " << obstacle.filename << std::endl;
        scene->at(obs)->getTransform()->rotate(obstacle.angle, obstacle.axis);
        obstacle.index = obs;

        if(obstacle.type == "ground")
            scene->at(obs)->setBody(Engine::Physics::Get().CreateBox({5.f, 0.5f, 5.f}, {0.f, 0.f, 0.f}, Engine::BodyType::Static));

    }
}


void Level::setGroundCollision(
    std::shared_ptr<Engine::Scene> scene, 
    Engine::Listener::Callback callback,
    unsigned user_index
)
{
   for (const Obstacle& obstacle : obstacles)
   {
       if(obstacle.type == "ground")
            Engine::Listener::Get().Add(scene, Engine::Listener::Event::ContactAdded, user_index, obstacle.index, callback);
   }
}