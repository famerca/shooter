#include <iostream>
#include <glm/gtx/io.hpp>

#include <GLS/GameObject.hpp>
#include <GLS/TransformComponent.hpp>

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
    }
}