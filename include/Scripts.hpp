#ifndef SCRIPTS_HPP
#define SCRIPTS_HPP
#include <GLS/ScriptComponent.hpp>

class PlataformaMovil: public Engine::ScriptComponent
{
public:

    void OnPhysicsUpdate(float dt) override;

};



#endif // SCRIPTS_HPP