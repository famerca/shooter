#include "Path.hpp"
#include <iostream>

namespace Engine {

    const fs::path& PathResolver::GetRoot() {
            static std::filesystem::path root = [] {
                #ifdef ENGINE_DEBUG_MODE
                    // CASO DEBUG:
                    // Usamos __FILE__ que devuelve la ruta absoluta de ESTE archivo
                    // Asumimos estructura: TuProyecto/src/Path.cpp
                    // Necesitamos subir 1 nivel para llegar a TuProyecto/
                    
                    std::filesystem::path thisFile = __FILE__;
                    
                    // Ajusta los .parent_path() según qué tan profundo esté este archivo
                    // Ejemplo: src/Path.cpp -> parent(src) -> Raíz
                    std::filesystem::path projectRoot = thisFile.parent_path().parent_path();
                    std::cout << "[PathResolver] ROOT_PATH: " << projectRoot.string() << std::endl;
                    return projectRoot;
                    
                #else
                    // CASO RELEASE:
                    // Intentamos detectar la raíz usando __FILE__ primero
                    std::filesystem::path thisFile = __FILE__;
                    std::filesystem::path projectRoot = thisFile.parent_path().parent_path();
                    
                    // Si no encontramos los recursos ahí, usamos el directorio de trabajo actual
                    // (útil si se ejecuta desde el directorio raíz del proyecto)
                    std::filesystem::path currentPath = std::filesystem::current_path();
                    if (std::filesystem::exists(projectRoot / "shaders") && std::filesystem::exists(projectRoot / "assets")) {
                        return projectRoot;
                    } else if (std::filesystem::exists(currentPath / "shaders") && std::filesystem::exists(currentPath / "assets")) {
                        return currentPath;
                    } else {
                        // Fallback: asumir que estamos en la raíz
                        return projectRoot;
                    }
                #endif
            }(); // El () al final ejecuta la lambda inmediatamente y guarda el resultado

            return root;
    }

    const fs::path& PathResolver::GetAssets() {
        static std::filesystem::path assets = GetRoot() / "assets";
        return assets;
    }

    const fs::path& PathResolver::GetShaders() {
        static std::filesystem::path shaders = GetRoot() / "shaders";
        return shaders;
    }

    const fs::path& PathResolver::GetTextures() {
        static std::filesystem::path textures = GetAssets() / "textures";
        return textures;
    }

    const fs::path& PathResolver::GetModels() {
        static std::filesystem::path models = GetAssets() / "models";
        return models;
    }

    const fs::path& PathResolver::GetAudios() {
        static std::filesystem::path audios = GetAssets() / "audios";
        return audios;
    }


}