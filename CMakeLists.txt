cmake_minimum_required(VERSION 3.20)
project(Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -Wpedantic -g)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)
find_package(assimp QUIET)


# --- 1. PREPARACIÓN DE DEPENDENCIAS (Motor y Jolt) ---

# Buscar la librería del Motor (GLS)
find_library(GLS_LIB
    NAMES GLS 
    PATHS ${PROJECT_SOURCE_DIR}/GLS/lib
    NO_DEFAULT_PATH
)

# Buscar la librería de Físicas (Jolt)
find_library(JOLT_LIBRARY
    NAMES Jolt 
    PATHS ${PROJECT_SOURCE_DIR}/GLS/lib
    NO_DEFAULT_PATH
)

find_library(RMLUI_LIBRARY
    NAMES rmlui 
    PATHS ${PROJECT_SOURCE_DIR}/GLS/lib
    NO_DEFAULT_PATH
)

find_library(RMLUI_DEBUGGER_LIBRARY
    NAMES rmlui_debugger 
    PATHS ${PROJECT_SOURCE_DIR}/GLS/lib
    NO_DEFAULT_PATH
)

if(NOT GLS_LIB OR NOT JOLT_LIBRARY)
    message(FATAL_ERROR "Faltan librerías: GLS=${GLS_LIB}, Jolt=${JOLT_LIBRARY}")
endif()

if(NOT RMLUI_LIBRARY OR NOT RMLUI_DEBUGGER_LIBRARY)
    message(FATAL_ERROR "Faltan librerías: RmlUi=${RMLUI_LIBRARY}, RmlUi_Debugger=${RMLUI_DEBUGGER_LIBRARY}")
endif()

# --- 2. CREACIÓN DE TU LIBRERÍA DE JUEGO (GameLib) ---

# A. Recolectar todos los archivos .cpp dentro de la carpeta /src
file(GLOB_RECURSE GAMELIB_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# B. Crear la librería estática con esos fuentes
add_library(GameLib STATIC ${GAMELIB_SOURCES})

# C. Configurar dónde están los .hpp (Includes)
# PUBLIC significa: "Yo (GameLib) los uso, y quien me use a mí (Game) también puede verlos"
target_include_directories(GameLib PUBLIC
    ${PROJECT_SOURCE_DIR}/include      # Tus cabeceras propias (inputManager.hpp, etc.) # Las cabeceras del motor
)

target_include_directories(GameLib SYSTEM PUBLIC
    ${PROJECT_SOURCE_DIR}/GLS/include 
    ${GLEW_INCLUDE_DIRS}               # Include directories de GLEW
)

# D. Definiciones de Preprocesador (Jolt y GLM)
# Las movemos aquí como PUBLIC para que apliquen tanto a la librería como al main
target_compile_definitions(GameLib PUBLIC
    JPH_PROFILE_ENABLED=1
    JPH_DEBUG_RENDERER=1
    JPH_OBJECT_STREAM=1
    GLM_ENABLE_EXPERIMENTAL
)

# --- 3. CREACIÓN DEL EJECUTABLE (Main) ---

add_executable(Game main.cpp)

# --- 4. LINKEO FINAL ---

# El ejecutable necesita tu lógica (GameLib), el motor (GLS) y las dependencias externas
target_link_libraries(Game PRIVATE
    GameLib             # <--- Tu nueva librería con inputManager, etc.
    ${GLS_LIB}          # El Motor
    ${JOLT_LIBRARY}     # Físicas
    ${RMLUI_DEBUGGER_LIBRARY}    # RmlUi
    ${RMLUI_LIBRARY}    # RmlUi
    Freetype::Freetype
    OpenGL::GL          # OpenGL (usando el target moderno de CMake)
    GLEW::GLEW          # GLEW (usando el target moderno de CMake)
    glfw 
    ${CMAKE_DL_LIBS}    # Para dlopen/dlsym si es necesario
)

# Linkear assimp si está disponible
if(TARGET assimp::assimp)
    target_link_libraries(Game PRIVATE assimp::assimp)
else()
    # Fallback: buscar la librería manualmente
    find_library(ASSIMP_LIBRARY
        NAMES assimp
        PATHS /opt/homebrew/lib /usr/local/lib
        NO_DEFAULT_PATH
    )
    if(ASSIMP_LIBRARY)
        target_link_libraries(Game PRIVATE ${ASSIMP_LIBRARY})
    else()
        # Último recurso: linkear directamente
        target_link_libraries(Game PRIVATE assimp)
    endif()
endif()


message(STATUS "Configuración completada. GameLib sources: ${GAMELIB_SOURCES}")