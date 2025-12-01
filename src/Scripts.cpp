#include <GLS/Body.hpp>
#include <GLS/TransformComponent.hpp>
#include <GLS/GameObject.hpp>
#include <GLS/Utils.hpp>

#include "Scripts.hpp"


PlataformaMovil::PlataformaMovil(scriptParams params)
{
    try {
        this->min = std::get<float>(params[0]);
        this->max = std::get<float>(params[1]);
        this->vertical = (std::get<std::string>(params[2]) == "vertical");
    }catch(const std::exception& e)
    {
        std::cout << "Error al crear plataforma movil: " << e.what() << std::endl;
    }
}


void PlataformaMovil::OnPhysicsUpdate(float dt)
{
    auto pos = body->GetPosition();

    float val = 0.f;

    if(vertical)
        val = pos.GetY();
    else
        val = pos.GetX();

    std::cout << "[PlataformaMovil] valor: " << val << std::endl;

    if(val <= min)
    {
        std::cout << "[PlataformaMovil] subiendo" << std::endl;
        if(vertical)
            body->SetVelocity({0.f, 1.f, 0.f});
        else
            body->SetVelocity({1.f, 0.f, 0.f});
    }else if(val >= max)
    {
        std::cout << "[PlataformaMovil] bajando" << std::endl;
        if(vertical)
            body->SetVelocity({0.f, -1.f, 0.f});
        else
            body->SetVelocity({-1.f, 0.f, 0.f});
  
    }
}


Enemy::Enemy(scriptParams params)
{
    rot_left = Engine::Utils::toQuant({0.f, 1.f, 0.f}, 90.f);
    rot_right = Engine::Utils::toQuant({0.f, 1.f, 0.f}, -90.f);

    try {
        this->min = std::get<float>(params[0]);
        this->max = std::get<float>(params[1]);
        this->speed = std::get<float>(params[2]);
    }catch(const std::exception& e)
    {
        std::cout << "Error al crear enemy: " << e.what() << std::endl;
    }
}

void Enemy::OnPhysicsUpdate(float dt)
{
    float x_pos = body->GetPosition().GetX();
    std::cout << "[Enemy] pos: " << x_pos << std::endl;
    if(x_pos <= min)
    {
        body->SetVelocity({speed, 0.f, 0.f});
        body->SetRotation(rot_left);
    }else if(x_pos >= max)
    {
        body->SetVelocity({-speed, 0.f, 0.f});
        body->SetRotation(rot_right);
    }
}


Parachute::Parachute(scriptParams params)
{
    try {
        this->is_coll = std::get<bool*>(params[0]);
        this->input = std::get<inputManager*>(params[1]);
        this->user = std::get<Engine::GameObject*>(params[2]);
    }catch(const std::exception& e)
    {
        std::cerr << "Error al crear parachute: " << e.what() << std::endl;
    }
}

void Parachute::OnPhysicsUpdate(float dt)
{

    if(hooked && input->is_holding())
    {
        user->getBody()->SetPosition(Engine::Utils::toGLMVec3(body->GetPosition()) + offset);
        user->getBody()->SetVelocity({0.f, 0.f, 0.f});
        body->SetVelocity(input->getForward() * speed);
        return;
    }else if(!input->is_holding())
        hooked = false;

    
    if(is_coll && input->is_holding())
    {
        hooked = true;
    }

}