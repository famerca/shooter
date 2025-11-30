#pragma once
#include <filesystem>

// Detectar modo DEBUG (Visual Studio usa _DEBUG, otros NDEBUG)
#ifndef PATH_HPP
#define PATH_HPP

namespace Engine {

    namespace fs = std::filesystem;
    
    class PathResolver {
    public:
        // Devuelve una referencia constante (static) para no recalcularla nunca
        static const std::filesystem::path& GetRoot();

        // Helper opcional para obtener assets directamente
        static const fs::path& GetAssets();

        static const fs::path& GetShaders();

        static const fs::path& GetTextures();

        static const fs::path& GetModels();

        static const fs::path& GetAudios();
    };

}

// --- LA MACRO MÁGICA ---
// Esto te permite usar ROOT_PATH en cualquier lugar como si fuera una variable
#define ROOT_PATH Engine::PathResolver::GetRoot()
#define ASSETS_PATH Engine::PathResolver::GetAssets()
#define SHADERS_PATH Engine::PathResolver::GetShaders()
#define TEXTURES_PATH Engine::PathResolver::GetTextures()
#define MODELS_PATH Engine::PathResolver::GetModels()
#define AUDIOS_PATH Engine::PathResolver::GetAudios()

#endif // PATH_HPP