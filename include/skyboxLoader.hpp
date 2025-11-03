// Archivo auxiliar o dentro de Skybox.hpp/cpp si lo prefieres

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>

#ifndef SKYBOX_LOADER_HPP
#define SKYBOX_LOADER_HPP

namespace fs = std::filesystem;

/**
 * @brief Clase auxiliar para construir el vector de caras del skybox en el orden correcto de OpenGL.
 */
class SkyboxLoader
{
public:
    fs::path local_path;

    SkyboxLoader(): face_map()
    {
        local_path = fs::path{__FILE__}.parent_path().parent_path();
    };

    ~SkyboxLoader() = default;



    // Mapea nombres intuitivos a la ruta del archivo
    void set_face(const std::string& name, const std::filesystem::path& path)
    {
        face_map[name] = path;
    }

    /**
     * @brief Genera el vector de caras ordenado para la función load_cubemap().
     * @return std::vector<std::filesystem::path> con las 6 caras ordenadas (PX, NX, PY, NY, PZ, NZ).
     */
    std::vector<std::filesystem::path> get_faces() const
    {
        std::vector<std::filesystem::path> ordered_faces;

        // El orden de OpenGL que necesita tu load_cubemap:
        // 0: GL_TEXTURE_CUBE_MAP_POSITIVE_X (+X) -> 'px'
        // 1: GL_TEXTURE_CUBE_MAP_NEGATIVE_X (-X) -> 'nx'
        // 2: GL_TEXTURE_CUBE_MAP_POSITIVE_Y (+Y) -> 'py'
        // 3: GL_TEXTURE_CUBE_MAP_NEGATIVE_Y (-Y) -> 'ny'
        // 4: GL_TEXTURE_CUBE_MAP_POSITIVE_Z (+Z) -> 'pz'
        // 5: GL_TEXTURE_CUBE_MAP_NEGATIVE_Z (-Z) -> 'nz'
        
        const std::vector<std::string> face_order = {
            "px", "nx", "py", "ny", "pz", "nz"
        };

        for (const auto& face_name : face_order)
        {
            auto it = face_map.find(face_name);
            if (it != face_map.end())
            {
                ordered_faces.push_back(it->second);
            }
            else
            {
                // Manejo de error si falta alguna cara
                std::cerr << "Error: Falta la cara del skybox: " << face_name << std::endl;
                // Devolver un vector vacío o lanzar excepción según sea necesario
                return {}; 
            }
        }
        
        if (ordered_faces.size() != 6)
        {
             std::cerr << "Error: Solo se encontraron " << ordered_faces.size() << " caras." << std::endl;
             return {};
        }

        return ordered_faces;
    }

private:
    // Mapa que almacena la ruta (path) de la imagen asociada a su nombre (string)
    std::map<std::string, std::filesystem::path> face_map;
};

#endif // SKYBOX_LOADER_HPP