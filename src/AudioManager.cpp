#include "AudioManager.hpp"

// Definimos la implementación de miniaudio SOLO en este archivo cpp
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "GameObject.hpp"
#include "AudioSourceComponent.hpp"
#include "AudioPlayer.hpp"

namespace Engine {

    AudioManager* AudioManager::instance = nullptr;

    AudioManager::AudioManager() {
        // Configurar ruta base (ajusta los parent_path según la ubicación de este .cpp)
        rootPath = std::filesystem::path(__FILE__).parent_path().parent_path() / "audios";
        engine = new ma_engine();
    }

    AudioManager::~AudioManager() {
        shutdown();
        delete engine;
    }

    AudioManager& AudioManager::Get() {
        if (instance == nullptr) {
            instance = new AudioManager();
        }
        return *instance;
    }

    void AudioManager::init() {
        if (ma_engine_init(NULL, engine) != MA_SUCCESS) {
            std::cerr << "Engine::AudioManager: Fallo al inicializar miniaudio." << std::endl;
        }
    }

    void AudioManager::shutdown() {
        for (size_t i = 0; i < audioLibrary.size(); i++) {
            clearAudio(i);
        }
        ma_engine_uninit(engine);
    }

    size_t AudioManager::addAudio(const std::string& filename) {
        AudioData newData;
        newData.relativePath = filename;
        newData.isLoaded = false;
        newData.soundInstance = nullptr;

        audioLibrary.push_back(newData);
        return audioLibrary.size() - 1; // Retorna el índice
    }

    void AudioManager::initAudio(size_t index) {
        if (index >= audioLibrary.size()) return;
        
        AudioData& data = audioLibrary[index];
        if (data.isLoaded) return; // Ya estaba cargado

        std::filesystem::path fullPath = rootPath / data.relativePath;
        
        // Reservar memoria para el objeto ma_sound
        data.soundInstance = new ma_sound();

        // Cargar archivo. Usamos flag DECODE para descomprimir en memoria RAM
        ma_result result = ma_sound_init_from_file(engine, fullPath.string().c_str(), 0, NULL, NULL, data.soundInstance);

        if (result != MA_SUCCESS) {
            std::cerr << "Engine::AudioManager: Error cargando audio " << fullPath << std::endl;
            delete data.soundInstance;
            data.soundInstance = nullptr;
            return;
        }

        data.isLoaded = true;
    }

     std::shared_ptr<AudioPlayer> AudioManager::make_audioPlayer(const std::string& path, float volume, bool loop) {
        // 1. Registramos el audio (si ya existe el path, addAudio podría optimizarse para devolver el existente,
        // pero para efectos de AudioPlayer global, usualmente es música única).
        size_t index = addAudio(path);
        
        // 2. Creamos y devolvemos la instancia (por valor, es ligero)
        // El constructor de AudioPlayer recibe el índice
        return std:: make_shared<AudioPlayer>(index, volume, loop);
    }

    std::shared_ptr<AudioSourceComponent> AudioManager::make_audioSource(std::shared_ptr<GameObject> owner, const std::string& path) {
        // 1. Añadir a la librería
        size_t index = addAudio(path);

        // 2. Crear el componente
        // Usamos make_shared para gestión automática de memoria
        auto source = std::make_shared<AudioSourceComponent>(owner);
        
        // 3. Asignar el audio
        source->setAudio(index);

        owner->addComponent(source);

        return source;
    }

    std::shared_ptr<AudioSourceComponent> AudioManager::clone_audioSource(std::shared_ptr<GameObject> owner, size_t originalIndex) {
        if (originalIndex >= audioLibrary.size()) {
            std::cerr << "Engine::AudioManager: Intento de clonar índice inválido." << std::endl;
            return nullptr;
        }

        // 1. Obtener el path del original
        std::string originalPath = audioLibrary[originalIndex].relativePath.string();

        // 2. IMPORTANTE: Forzamos añadir una NUEVA entrada al vector, aunque el path sea el mismo.
        // Esto nos dará un nuevo 'AudioData' con su propio 'ma_sound*' independiente.
        // Esto permite que suene encima del anterior sin cortarlo.
        size_t newIndex = addAudio(originalPath);

        // 3. Crear el componente con este NUEVO índice
        auto source = std::make_shared<AudioSourceComponent>(owner);
        source->setAudio(newIndex);

        owner->addComponent(source);
        
        // Opcional: Copiar configuraciones del original si quisieras (volumen, loop),
        // pero como solo tenemos el índice, asumimos valores por defecto.

        return source;
    }

    void AudioManager::clearAudio(size_t index) {
        if (index >= audioLibrary.size()) return;

        AudioData& data = audioLibrary[index];
        if (!data.isLoaded || data.soundInstance == nullptr) return;

        // Desinicializar miniaudio y liberar memoria C++
        ma_sound_uninit(data.soundInstance);
        delete data.soundInstance;
        
        data.soundInstance = nullptr;
        data.isLoaded = false;
    }

    void AudioManager::playAudio(size_t index) {
        if (index >= audioLibrary.size()) return;

        // Inicialización perezosa (Lazy Init)
        if (!audioLibrary[index].isLoaded) {
            initAudio(index);
        }

        if (audioLibrary[index].isLoaded) {
            // Reiniciar si ya estaba sonando (opcional, depende de gameplay)
            if (ma_sound_is_playing(audioLibrary[index].soundInstance)) {
                 ma_sound_seek_to_pcm_frame(audioLibrary[index].soundInstance, 0);
            }
            ma_sound_start(audioLibrary[index].soundInstance);
        }
    }

    void AudioManager::stopAudio(size_t index) {
        if (index < audioLibrary.size() && audioLibrary[index].isLoaded) {
            ma_sound_stop(audioLibrary[index].soundInstance);
        }
    }
    
    bool AudioManager::isPlaying(size_t index) {
        if (index < audioLibrary.size() && audioLibrary[index].isLoaded) {
             return ma_sound_is_playing(audioLibrary[index].soundInstance);
        }
        return false;
    }

    // --- Setters Wrappers ---

    void AudioManager::setSoundPosition(size_t index, const glm::vec3& pos) {
        if (index < audioLibrary.size() && audioLibrary[index].isLoaded) {
            ma_sound_set_position(audioLibrary[index].soundInstance, pos.x, pos.y, pos.z);
        }
    }

    void AudioManager::setSoundVolume(size_t index, float volume) {
        if (index < audioLibrary.size() && audioLibrary[index].isLoaded) {
            ma_sound_set_volume(audioLibrary[index].soundInstance, volume);
        }
    }

    void AudioManager::setSoundLooping(size_t index, bool loop) {
        if (index < audioLibrary.size() && audioLibrary[index].isLoaded) {
            ma_sound_set_looping(audioLibrary[index].soundInstance, loop ? MA_TRUE : MA_FALSE);
        }
    }

    void AudioManager::setSoundDistance(size_t index, float minDist, float maxDist) {
        if (index < audioLibrary.size() && audioLibrary[index].isLoaded) {
            ma_sound_set_min_distance(audioLibrary[index].soundInstance, minDist);
            ma_sound_set_max_distance(audioLibrary[index].soundInstance, maxDist);
        }
    }

    void AudioManager::setSoundSpatialization(size_t index, bool enable) {
        if (index < audioLibrary.size() && audioLibrary[index].isLoaded) {
            ma_sound_set_spatialization_enabled(audioLibrary[index].soundInstance, enable ? MA_TRUE : MA_FALSE);
        }
    }

    void AudioManager::setListenerPosition(const glm::vec3& pos) {
        ma_engine_listener_set_position(engine, 0, pos.x, pos.y, pos.z);
    }

    void AudioManager::setListenerDirection(const glm::vec3& forward) {
        ma_engine_listener_set_direction(engine, 0, forward.x, forward.y, forward.z);
    }

}
