#include <GLS/Body.hpp>
#include <GLS/TransformComponent.hpp>
#include <GLS/GameObject.hpp>

#include "Scripts.hpp"


void PlataformaMovil::OnPhysicsUpdate(float dt)
{
    auto pos = body->GetPosition();

    if(pos.GetY() <= 0.5)
    {
        body->SetVelocity({0.f, 1.f, 0.f});
    }else if(pos.GetY() >= 5)
    {
        body->SetVelocity({0.f, -1.f, 0.f});
    }
}