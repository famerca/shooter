cmake_minimum_required(VERSION 3.20)

# Project name
project(GLShooter)

# Set C++ Required version
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)

# Set compiler flags
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings -Wno-parentheses -Wfloat-equal -pedantic ${CMAKE_CXX_FLAGS}")

# Use -std=c++XX instead of -std=gnu++XX
set(CMAKE_CXX_EXTENSIONS OFF)

# Set dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# macOS specific: Find OpenGL framework
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    
    # Set GLEW paths for macOS
    set(GLEW_INCLUDE_DIRS "/opt/homebrew/include")
    set(GLEW_LIBRARIES "/opt/homebrew/lib/libGLEW.dylib")
    
    # Set assimp paths for macOS
    find_library(ASSIMP_LIBRARY assimp PATHS "/opt/homebrew/lib" NO_DEFAULT_PATH)
    find_path(ASSIMP_INCLUDE_DIR assimp/ai_assert.h PATHS "/opt/homebrew/include" NO_DEFAULT_PATH)
endif()

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

# Incluir headers de tu proyecto y de Jolt
target_include_directories(GLSHOOTERLIB PUBLIC "${PROJECT_SOURCE_DIR}/include" ${JOLT_INCLUDE_DIR})

# 3. Encontrar y Enlazar Jolt (incluyendo math y threading)
find_library(JOLT_LIBRARY NAMES Jolt PATHS ${JOLT_LIB_DIR})

if(NOT JOLT_LIBRARY)
    message(FATAL_ERROR "No se pudo encontrar la librería Jolt.a en ${JOLT_LIB_DIR}")
endif()

# Enlazar dependencias del motor (GLFW, GLEW, etc.) y de Jolt
if(APPLE)
    # Dependencias de macOS
    target_link_libraries(GLSHOOTERLIB PUBLIC ${OPENGL_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY} ${GLEW_LIBRARIES} glfw ${ASSIMP_LIBRARY} ${JOLT_LIBRARY} m pthread)
    target_include_directories(GLSHOOTERLIB PUBLIC ${ASSIMP_INCLUDE_DIR})
else()
    # Dependencias de Linux/Unix
    target_link_libraries(GLSHOOTERLIB PUBLIC GL GLEW glfw assimp ${JOLT_LIBRARY} m pthread)
endif()


# --- DECLARACIÓN Y ENLAZADO DEL EJECUTABLE ---

# 4. Declarar el ejecutable (Esto debe ir antes del target_link_libraries de GLSooter)
add_executable(GLShooter main.cpp)

# 5. Enlazar el ejecutable a la librería principal (GLSHOOTERLIB ya tiene todas las dependencias)
target_link_libraries(GLShooter
    PRIVATE
        GLSHOOTERLIB
)