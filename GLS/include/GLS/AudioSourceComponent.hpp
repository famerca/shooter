#pragma once
#include "Component.hpp"
#include <string>

#ifndef AUDIO_SOURCE_COMPONENT_HPP
#define AUDIO_SOURCE_COMPONENT_HPP

namespace Engine {

    class AudioSourceComponent : public Component {
    private:
        size_t audioIndex;      // Índice en el AudioManager
        bool hasAudio;          // Flag de seguridad
        
        // Configuración local
        bool playOnAwake;
        bool loop;
        float volume;
        float minDistance;
        float maxDistance;

    public:
        AudioSourceComponent(Owner owner, bool playOnAwake = false, bool loop = false, 
                             float volume = 1.0f, float minDesc = 1.0f, float maxDist = 20.0f);

        // Asigna el audio usando el índice que te dio AudioManager::addAudio
        void setAudio(size_t index);

        void play();
        void stop();
        
        // Setters que actualizan inmediatamente el engine
        void setVolume(float vol);
        void setLoop(bool loop);
        void setDistance(float min, float max);

        void update(const float& dt) override;
    };
}

#endif // AUDIO_SOURCE_COMPONENT_HPP