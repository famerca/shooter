cmake_minimum_required(VERSION 3.20)

# Project name
project(GLSX LANGUAGES CXX)

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
find_package(Freetype REQUIRED)

# macOS specific: Find frameworks
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    
    # Set GLEW/Assimp paths for macOS (Asumiendo Homebrew)
    find_library(GLEW_LIBRARY GLEW PATHS "/opt/homebrew/lib" NO_DEFAULT_PATH)
    find_path(GLEW_INCLUDE_DIR GL/glew.h PATHS "/opt/homebrew/include" NO_DEFAULT_PATH)
    find_library(ASSIMP_LIBRARY assimp PATHS "/opt/homebrew/lib" NO_DEFAULT_PATH)
    find_path(ASSIMP_INCLUDE_DIR assimp/ai_assert.h PATHS "/opt/homebrew/include" NO_DEFAULT_PATH)
    
    # Set GLEW_LIBRARIES if not set by find_package
    if(NOT GLEW_LIBRARIES AND GLEW_LIBRARY)
        set(GLEW_LIBRARIES ${GLEW_LIBRARY})
    endif()
endif()

# --- Terceros ---
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/third_party/BSlogger)
    execute_process(
        COMMAND git clone "https://github.com/bschiffthaler/BSlogger.git" BSlogger
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party
        ERROR_QUIET
    )
endif()
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/third_party/stb)
    execute_process(
        COMMAND git clone "https://github.com/nothings/stb.git" stb
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party
        ERROR_QUIET
    )
endif()

# Descargar miniaudio si no existe
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio)
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio/miniaudio.h)
    execute_process(
        COMMAND curl -L "https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h" -o miniaudio.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio
        ERROR_QUIET
    )
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party/BSlogger/src ${CMAKE_CURRENT_BINARY_DIR}/third_party/stb ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio)

# --- CONFIGURACIÓN RMLUI ---
set(RMLUI_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/RmlUi)
set(RMLUI_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/RmlUi-build)

# Descargar RmlUi si no existe
if(NOT EXISTS ${RMLUI_DIR})
    execute_process(
        COMMAND git clone --depth 1 "https://github.com/mikke89/RmlUi.git" RmlUi
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party
        ERROR_QUIET
    )
endif()

# Configurar y compilar RmlUi
if(EXISTS ${RMLUI_DIR}/CMakeLists.txt)
    set(RMLUI_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(RMLUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(RMLUI_BUILD_INCLUDE_TOOLS OFF CACHE BOOL "" FORCE)
    
    add_subdirectory(${RMLUI_DIR} ${RMLUI_BUILD_DIR})
endif()

# --- INICIO CONFIGURACIÓN JOLT ---
set(VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)

# 1. Definir rutas de Jolt
find_path(JOLT_INCLUDE_DIR Jolt/Jolt.h PATHS ${VENDOR_DIR}/JoltPhysics/include)
set(JOLT_LIB_DIR ${VENDOR_DIR}/JoltPhysics/lib)

# 2. Declarar la librería del motor
# En Linux, usar STATIC para evitar problemas con libJolt.a que no tiene -fPIC
# En macOS, usar SHARED como antes
file(GLOB SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(GLSHOOTERLIB SHARED ${SRC})

# 🔑 SINCRONIZACIÓN JOLT: Definiciones requeridas por Jolt en modo Debug
target_compile_definitions(GLSHOOTERLIB PUBLIC
    JPH_PROFILE_ENABLED=1
    JPH_DEBUG_RENDERER=1
    JPH_OBJECT_STREAM=1
)

# Definir ENGINE_DEBUG_MODE cuando está en modo Debug para resolver rutas correctamente
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(GLSHOOTERLIB PUBLIC ENGINE_DEBUG_MODE)
endif()

# RTTI: Jolt fue compilado sin RTTI, pero RmlUi lo requiere
# Deshabilitar RTTI solo para HitboxRenderer que usa Jolt
set_source_files_properties(src/HitboxRenderer.cpp PROPERTIES COMPILE_FLAGS "-fno-rtti")

# 3. Incluir headers de tu proyecto y de Jolt
target_include_directories(GLSHOOTERLIB PUBLIC 
    "${PROJECT_SOURCE_DIR}/include" 
    ${JOLT_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio
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
    if(GLEW_INCLUDE_DIR)
        target_include_directories(GLSHOOTERLIB PUBLIC ${GLEW_INCLUDE_DIR})
    endif()
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

# Enlazar RmlUi y sus dependencias
if(TARGET rmlui_core)
    # Establecer orden de compilación: rmlui_core -> rmlui_debugger -> GLSHOOTERLIB
    if(TARGET rmlui_debugger)
        add_dependencies(rmlui_debugger rmlui_core)
        add_dependencies(GLSHOOTERLIB rmlui_debugger)
    else()
        add_dependencies(GLSHOOTERLIB rmlui_core)
    endif()
    
    # Enlazar librerías
    target_link_libraries(GLSHOOTERLIB PUBLIC 
        rmlui_core
        ${FREETYPE_LIBRARIES}
    )
    # Agregar archivos fuente del backend GLFW_GL3 directamente
    target_sources(GLSHOOTERLIB PRIVATE
        ${RMLUI_DIR}/Backends/RmlUi_Platform_GLFW.cpp
        ${RMLUI_DIR}/Backends/RmlUi_Renderer_GL3.cpp
    )
    # En macOS, necesitamos dlopen para el renderer GL3
    if(UNIX)
        target_link_libraries(GLSHOOTERLIB PUBLIC ${CMAKE_DL_LIBS})
    endif()
    # Enlazar rmlui_debugger si está disponible (herramientas de depuración visual)
    if(TARGET rmlui_debugger)
        target_link_libraries(GLSHOOTERLIB PUBLIC rmlui_debugger)
    endif()
    # Agregar includes de RmlUi y FreeType
    target_include_directories(GLSHOOTERLIB PUBLIC 
        ${RMLUI_DIR}/Include
        ${RMLUI_DIR}/Backends
        ${FREETYPE_INCLUDE_DIRS}
    )
endif()

# --- DECLARACIÓN Y ENLAZADO DEL EJECUTABLE ---

# 5. Declarar el ejecutable
add_executable(GLSX main.cpp)

# Definir ENGINE_DEBUG_MODE para el ejecutable también
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(GLSX PRIVATE ENGINE_DEBUG_MODE)
endif()

#target_compile_options(GLSX PRIVATE -fno-rtti)

# 6. Enlazar el ejecutable a la librería principal
target_link_libraries(GLSX
    PRIVATE
        GLSHOOTERLIB
)