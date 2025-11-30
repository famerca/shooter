#include <GLS/Body.hpp>
#include <GLS/TransformComponent.hpp>
#include <GLS/GameObject.hpp>

#include "Scripts.hpp"


PlataformaMovil::PlataformaMovil(scriptParams params)
{
    try {
        this->max = std::get<float>(params[0]);
        this->min = std::get<float>(params[1]);
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

    if(val <= min)
    {
        body->SetVelocity({0.f, 1.f, 0.f});
    }else if(val >= max)
    {
        body->SetVelocity({0.f, -1.f, 0.f});
    }
}