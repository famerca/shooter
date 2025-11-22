#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <memory>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>

#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

// Forward declaration de miniaudio para no ensuciar el header
struct ma_engine;
struct ma_sound;
class GameObject;

namespace Engine {

    class AudioPlayer;
    class AudioSourceComponent;

    struct AudioData {
        std::filesystem::path relativePath;
        ma_sound* soundInstance = nullptr; // Puntero para manejo manual de memoria
        bool isLoaded = false;
    };

    class AudioManager {
    private:
        static AudioManager* instance;
        ma_engine* engine;
        std::vector<AudioData> audioLibrary;
        std::filesystem::path rootPath;

        // Constructor privado (Singleton)
        AudioManager();
        ~AudioManager();

    public:
        // Singleton access
        static AudioManager& Get();

        // Inicialización del motor
        void init();
        void shutdown();

        // Gestión de recursos
        // Retorna el índice del audio agregado
        size_t addAudio(const std::string& filename);
        
        // Carga el sonido en memoria (RAM)
        void initAudio(size_t index);
        
        // Libera la memoria del sonido pero mantiene el índice reservado
        void clearAudio(size_t index);

        // Control de reproducción
        void playAudio(size_t index);
        void stopAudio(size_t index);
        bool isPlaying(size_t index);

        // Configuración de parámetros del sonido (llamado por Componentes)
        void setSoundPosition(size_t index, const glm::vec3& pos);
        void setSoundVolume(size_t index, float volume);
        void setSoundLooping(size_t index, bool loop);
        void setSoundDistance(size_t index, float minDesc, float maxDist);
        void setSoundSpatialization(size_t index, bool enable);

        // Control del Listener (Oído global)
        void setListenerPosition(const glm::vec3& pos);
        void setListenerDirection(const glm::vec3& forward);

        //Auxiliares para crear componentes AudioSource

        std::shared_ptr<AudioPlayer> make_audioPlayer(const std::string& path, float volume = 1.0f, bool loop = false);

        // Crea un componente AudioSource adjunto a un Owner
        std::shared_ptr<AudioSourceComponent> make_audioSource(std::shared_ptr<GameObject> owner, const std::string& path);

        // CLONAR: Crea una nueva entrada en la librería (copia del path original)
        // y devuelve un nuevo componente con su PROPIA instancia de sonido.
        // Ideal para disparos, pasos o sonidos repetitivos que se solapan.
        std::shared_ptr<AudioSourceComponent> clone_audioSource(std::shared_ptr<GameObject> owner, size_t originalIndex);

    };
}

#endif // AUDIO_MANAGER_HPP