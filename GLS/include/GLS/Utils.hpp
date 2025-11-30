#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <iomanip>

#include <glm/glm.hpp>
#include <Jolt/Jolt.h>

namespace Engine
{
class Utils
{
public:
    static void printMat4(const glm::mat4& matrix) {
        std::cout << "------------------------------------------\n";
        std::cout << "|             glm::mat4 print            |\n";
        std::cout << "------------------------------------------\n";

        // Establece el formato de impresión para mejor legibilidad
        // fixed: usa notación de punto flotante estándar
        // setprecision: muestra 4 decimales
        std::cout << std::fixed << std::setprecision(4);

        // GLM almacena los datos en orden Column-Major (columna principal),
        // pero la imprimimos en formato Row-Major (Fila x Columna) para mayor claridad.
        
        for (int i = 0; i < 4; ++i) { // Iterar sobre las filas (i)
            std::cout << "| ";
            for (int j = 0; j < 4; ++j) { // Iterar sobre las columnas (j)
                // matrix[j][i] accede al elemento de la columna j, fila i
                // Usamos [j][i] para mostrarlo en el orden visual de Filas x Columnas.
                std::cout << std::setw(10) << matrix[j][i] << " ";
            }
            std::cout << "|\n";
        }

        std::cout << "------------------------------------------\n";
    }


    static JPH::Vec3 toJoltVec3(const glm::vec3& vec3)
    {
        return JPH::Vec3(vec3.x, vec3.y, vec3.z);
    }

    static glm::vec3 toGLMVec3(const JPH::Vec3& vec3)
    {
        return glm::vec3(vec3.GetX(), vec3.GetY(), vec3.GetZ());
    }
};

}
#endif // UTILS_HPP