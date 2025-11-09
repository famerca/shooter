#pragma once

#include <Jolt/Jolt.h>
#include <iostream>

namespace JoltTest {

// Definimos una función tipo C compatible con el puntero JPH::Trace
inline void JoltTraceImpl(const char* fmt, ...) {
    std::cout << "[Jolt] Trace llamada correctamente." << std::endl;
}

inline void RunBasicTest() {
    // Inicializa el sistema de asignación de memoria (requerido por Jolt)
    JPH::RegisterDefaultAllocator();

    // Asigna nuestra función de traza
    JPH::Trace = &JoltTraceImpl;

    // Prueba mínima: crear un vector 3D
    JPH::Vec3 v(1.0f, 2.0f, 3.0f);
    std::cout << "Vector de prueba: "
              << v.GetX() << ", "
              << v.GetY() << ", "
              << v.GetZ() << std::endl;

    std::cout << "✅ Jolt Physics está correctamente incluido y linkeado." << std::endl;
}

} // namespace JoltTest
