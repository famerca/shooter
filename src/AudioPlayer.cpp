#include "AudioPlayer.hpp"
#include "AudioManager.hpp"

namespace Engine {

    AudioPlayer::AudioPlayer(size_t index, float volume, bool loop) 
        : volume(volume), loop(loop), hasAudio(false)
    {
        if (index != static_cast<size_t>(-1)) {
            setAudio(index);
        }
    }

    void AudioPlayer::setAudio(size_t index) {
        this->audioIndex = index;
        this->hasAudio = true;

        // Aseguramos que esté cargado
        AudioManager::Get().initAudio(audioIndex);

        // Configuración 2D
        AudioManager::Get().setSoundSpatialization(audioIndex, false); // Desactivar 3D
        AudioManager::Get().setSoundVolume(audioIndex, volume);
        AudioManager::Get().setSoundLooping(audioIndex, loop);
    }

    void AudioPlayer::play() {
        if (hasAudio) AudioManager::Get().playAudio(audioIndex);
    }

    void AudioPlayer::stop() {
        if (hasAudio) AudioManager::Get().stopAudio(audioIndex);
    }

    void AudioPlayer::setVolume(float vol) {
        this->volume = vol;
        if (hasAudio) AudioManager::Get().setSoundVolume(audioIndex, vol);
    }

    void AudioPlayer::setLoop(bool val) {
        this->loop = val;
        if (hasAudio) AudioManager::Get().setSoundLooping(audioIndex, val);
    }
}