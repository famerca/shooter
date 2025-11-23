#include "Path.hpp"

namespace Engine {

    const fs::path& PathResolver::GetRoot() {
            static std::filesystem::path root = [] {
                #ifdef ENGINE_DEBUG_MODE
                    // CASO DEBUG:
                    // Usamos __FILE__ que devuelve la ruta absoluta de ESTE archivo header (.hpp)
                    // Asumimos estructura: TuProyecto/src/Core/Paths.hpp
                    // Necesitamos subir 2 niveles para llegar a TuProyecto/
                    
                    std::filesystem::path thisFile = __FILE__;
                    
                    // Ajusta los .parent_path() según qué tan profundo esté este archivo
                    // Ejemplo: src/Core/Paths.hpp -> parent(Core) -> parent(src) -> Raíz
                    return thisFile.parent_path().parent_path(); 
                    srd::cout << "[PathResolver] ROOT_PATH: " << root.string() << std::endl;
                    
                #else
                    // CASO RELEASE:
                    // Usamos la ruta donde está el ejecutable (o el working directory)
                    // Se asume que en release la carpeta 'assets' está junto al .exe
                    return std::filesystem::current_path();
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