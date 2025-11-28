#ifndef LEVEL_HPP
#define LEVEL_HPP
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <GLS/Listener.hpp>
#include <GLS/Scene.hpp>

#include "Obstacle.hpp"


struct ObstacleInit
{
    std::string filename;
    std::string tag;
    glm::vec3 pos;
    ObstacleSettings settings;
};

class Level
{
    private:
        std::vector<std::shared_ptr<Obstacle>> obstacles;
        std::shared_ptr<Engine::Scene> scene;
        unsigned user_index;
    
    public:
        using init_list = std::initializer_list<ObstacleInit>;
        Level(std::shared_ptr<Engine::Scene> scene, unsigned user_index);
        ~Level();

        void init(const init_list& list);
};



#endif