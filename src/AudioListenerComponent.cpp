#include "AudioListenerComponent.hpp"
#include "AudioManager.hpp"
#include "TransformComponent.hpp"
#include "GameObject.hpp"

namespace Engine {

    AudioListenerComponent::AudioListenerComponent(Owner owner) 
        : Component(owner, Component::Type::AudioListener), relativePosition(0.0f) {}

    void AudioListenerComponent::setRelativePosition(const glm::vec3& pos) {
        this->relativePosition = pos;
    }

    void AudioListenerComponent::update(const float& dt) {
        if (!owner || !owner->getTransform()) return;

        // Calcular posición absoluta
        glm::vec3 finalPos = owner->getTransform()->getPosition() + relativePosition;

        AudioManager::Get().setListenerPosition(finalPos);
    }
}