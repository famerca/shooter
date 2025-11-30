#ifndef SCRIPTS_HPP
#define SCRIPTS_HPP
#include <GLS/ScriptComponent.hpp>

#include "Obstacle.hpp"

class PlataformaMovil: public Engine::ScriptComponent
{
public:
    float speed = 0.f;
    bool vertical = true;
    int max{5};
    int min{0};

    PlataformaMovil() = default;

    PlataformaMovil(scriptParams params);

    void OnPhysicsUpdate(float dt) override;

};



#endif // SCRIPTS_HPP