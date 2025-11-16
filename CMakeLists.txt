cmake_minimum_required(VERSION 3.20)

# Project name
project(GLShooter LANGUAGES CXX)

# Set C++ Required version
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF) # Use -std=c++XX instead of -std=gnu++XX

# --- Configuración de Compilación ---
# Establecer el modo Debug para que funcione con la librería Jolt Debug.
set(CMAKE_BUILD_TYPE Debug) 
add_compile_options(-Wall -Wextra -Wpedantic -g) # Unificado las opciones aquí

# Set dependencies (OpenGL, GLEW, etc.)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# macOS specific: Find frameworks
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    
    # Set GLEW/Assimp paths for macOS (Asumiendo Homebrew)
    find_library(ASSIMP_LIBRARY assimp PATHS "/opt/homebrew/lib" NO_DEFAULT_PATH)
    find_path(ASSIMP_INCLUDE_DIR assimp/ai_assert.h PATHS "/opt/homebrew/include" NO_DEFAULT_PATH)
endif()

# --- Terceros ---
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)
execute_process(
    COMMAND git clone "https://github.com/bschiffthaler/BSlogger.git" BSlogger
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)
execute_process(
    COMMAND git clone "https://github.com/nothings/stb.git" stb
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party/BSlogger/src ${CMAKE_CURRENT_BINARY_DIR}/third_party/stb)

# --- INICIO CONFIGURACIÓN JOLT ---
set(VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)

# 1. Definir rutas de Jolt
find_path(JOLT_INCLUDE_DIR Jolt/Jolt.h PATHS ${VENDOR_DIR}/JoltPhysics/include)
set(JOLT_LIB_DIR ${VENDOR_DIR}/JoltPhysics/lib)

# 2. Declarar la librería del motor
file(GLOB SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(GLSHOOTERLIB ${SRC})

# 🔑 SINCRONIZACIÓN JOLT: Definiciones requeridas por Jolt en modo Debug
target_compile_definitions(GLSHOOTERLIB PUBLIC
    JPH_PROFILE_ENABLED=1
    JPH_DEBUG_RENDERER=1
    JPH_OBJECT_STREAM=1
)

target_compile_options(GLSHOOTERLIB PRIVATE -fno-rtti)

# 3. Incluir headers de tu proyecto y de Jolt
target_include_directories(GLSHOOTERLIB PUBLIC 
    "${PROJECT_SOURCE_DIR}/include" 
    ${JOLT_INCLUDE_DIR}
)

# 4. Encontrar y Enlazar Jolt (incluyendo math y threading)
# CRUCIAL: Busca tanto "Jolt" como "Jolt_d" (Debug)
find_library(JOLT_LIBRARY NAMES Jolt Jolt_d PATHS ${JOLT_LIB_DIR})

if(NOT JOLT_LIBRARY)
    message(FATAL_ERROR "No se pudo encontrar la librería Jolt en ${JOLT_LIB_DIR}")
endif()

# Enlazar dependencias
if(APPLE)
    target_link_libraries(GLSHOOTERLIB PUBLIC 
        ${OPENGL_LIBRARY} 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY} 
        ${GLEW_LIBRARIES} 
        glfw 
        ${ASSIMP_LIBRARY} 
        ${JOLT_LIBRARY} 
        m 
        pthread
    )
    target_include_directories(GLSHOOTERLIB PUBLIC ${ASSIMP_INCLUDE_DIR})
else()
    # Dependencias de Linux/Unix
    target_link_libraries(GLSHOOTERLIB PUBLIC 
        GL 
        GLEW 
        glfw 
        assimp 
        ${JOLT_LIBRARY} 
        m 
        pthread
    )
endif()

# --- DECLARACIÓN Y ENLAZADO DEL EJECUTABLE ---

# 5. Declarar el ejecutable
add_executable(GLShooter main.cpp)

#target_compile_options(GLShooter PRIVATE -fno-rtti)

# 6. Enlazar el ejecutable a la librería principal
target_link_libraries(GLShooter
    PRIVATE
        GLSHOOTERLIB
)