#pragma once
#include <cstddef>

#ifndef AUDIO_PLAYER_HPP
#define AUDIO_PLAYER_HPP

namespace Engine {

    class AudioPlayer {
    private:
        size_t audioIndex;
        bool hasAudio;
        float volume;
        bool loop;

    public:
        // Constructor con valores por defecto
        AudioPlayer(size_t index = -1, float volume = 1.0f, bool loop = false);

        void setAudio(size_t index);
        void play();
        void stop();
        
        void setVolume(float volume);
        void setLoop(bool loop);
    };
}

#endif // AUDIO_PLAYER_HPP