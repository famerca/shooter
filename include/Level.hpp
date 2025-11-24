#ifndef LEVEL_HPP
#define LEVEL_HPP
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <GLS/Scene.hpp>


struct Obstacle
{
    std::string filename;
    std::string type;
    glm::vec3 pos;
    glm::vec3 scale;
};

class Level
{
    private:
        std::vector<Obstacle> obstacles;
    
    public:
        using init_list = std::initializer_list<Obstacle>;
        Level(const init_list& list);
        ~Level();

        void init(std::shared_ptr<Engine::Scene> scene);
};



#endif