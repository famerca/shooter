#include "AudioSourceComponent.hpp"
#include "AudioManager.hpp"
#include "TransformComponent.hpp"
#include "GameObject.hpp"

namespace Engine {

    AudioSourceComponent::AudioSourceComponent(Owner owner, bool playOnAwake, bool loop, float volume, float minDesc, float maxDist)
        : Component(owner, Component::Type::AudioSource),
          playOnAwake(playOnAwake), loop(loop), volume(volume), 
          minDistance(minDesc), maxDistance(maxDist), hasAudio(false), audioIndex(0)
    {
    }

    void AudioSourceComponent::setAudio(size_t index) {
        this->audioIndex = index;
        this->hasAudio = true;

        // Forzamos init para poder setear parámetros antes de que suene
        AudioManager::Get().initAudio(audioIndex);
        
        // Aplicamos la configuración inicial al motor
        AudioManager::Get().setSoundSpatialization(audioIndex, true); // Es 3D
        AudioManager::Get().setSoundVolume(audioIndex, volume);
        AudioManager::Get().setSoundLooping(audioIndex, loop);
        AudioManager::Get().setSoundDistance(audioIndex, minDistance, maxDistance);

        if (playOnAwake) {
            play();
        }
    }

    void AudioSourceComponent::play() {
        if(hasAudio) AudioManager::Get().playAudio(audioIndex);
    }

    void AudioSourceComponent::stop() {
        if(hasAudio) AudioManager::Get().stopAudio(audioIndex);
    }

    void AudioSourceComponent::setVolume(float vol) {
        this->volume = vol;
        if(hasAudio) AudioManager::Get().setSoundVolume(audioIndex, vol);
    }
    
    void AudioSourceComponent::setLoop(bool val) {
        this->loop = val;
        if(hasAudio) AudioManager::Get().setSoundLooping(audioIndex, val);
    }

    void AudioSourceComponent::setDistance(float min, float max) {
        this->minDistance = min;
        this->maxDistance = max;
        if(hasAudio) AudioManager::Get().setSoundDistance(audioIndex, min, max);
    }

    void AudioSourceComponent::update(const float& dt) {
        if (!hasAudio || !owner || !owner->getTransform()) return;

        // Solo actualizamos la posición si el audio se está reproduciendo 
        // (optimización pequeña, miniaudio lo maneja bien de todas formas)
        if (AudioManager::Get().isPlaying(audioIndex)) {
            glm::vec3 pos = owner->getTransform()->getPosition();
            AudioManager::Get().setSoundPosition(audioIndex, pos);
        }
    }
}