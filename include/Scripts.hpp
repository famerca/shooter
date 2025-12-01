#ifndef SCRIPTS_HPP
#define SCRIPTS_HPP
#include <GLS/ScriptComponent.hpp>
#include <GLS/Utils.hpp>


#include "Obstacle.hpp"
#include "inputManager.hpp"

class PlataformaMovil: public Engine::ScriptComponent
{
    float speed = 0.f;
    bool vertical = true;
    int max{5};
    int min{0};

    public:

    PlataformaMovil() = default;

    PlataformaMovil(scriptParams params);

    void OnPhysicsUpdate(float dt) override;

};

class Enemy: public Engine::ScriptComponent
{

    float speed = 1.f;
    int max{5};
    int min{0};
    glm::quat rot_left{};
    glm::quat rot_right{};

    public:

    Enemy() = default;

    Enemy(scriptParams params);

    void OnPhysicsUpdate(float dt) override;

};

class Parachute: public Engine::ScriptComponent
{
    float speed = 5.f;
    bool *is_coll{nullptr};
    inputManager *input{nullptr};
    Engine::GameObject *user{nullptr};
    glm::vec3 offset{0.f, -2.2f, 0.f};
    bool hooked{false};

    public:
    Parachute() = default;
    Parachute(scriptParams params);
    void OnPhysicsUpdate(float dt) override;
};


#endif // SCRIPTS_HPP