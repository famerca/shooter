#pragma once
#include "Component.hpp"
#include <glm/glm.hpp>

#ifndef AUDIO_LISTENER_COMPONENT_HPP
#define AUDIO_LISTENER_COMPONENT_HPP

namespace Engine {

    class AudioListenerComponent : public Component {
    private:
        glm::vec3 relativePosition; // Offset respecto al GameObject

    public:
        AudioListenerComponent(Owner owner); 
        
        void setRelativePosition(const glm::vec3& pos);
        
        // Actualiza la posición del engine en cada frame
        void update(const float& dt) override;
    };
}

#endif // AUDIO_LISTENER_COMPONENT_HPP