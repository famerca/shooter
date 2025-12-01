#include <GLS/Body.hpp>
#include <GLS/TransformComponent.hpp>
#include <GLS/GameObject.hpp>

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

