cmake_minimum_required(VERSION 3.20)
project(GLSX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -Wpedantic -g)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    
    find_library(GLEW_LIBRARY GLEW PATHS "/opt/homebrew/lib" NO_DEFAULT_PATH)
    find_path(GLEW_INCLUDE_DIR GL/glew.h PATHS "/opt/homebrew/include" NO_DEFAULT_PATH)
    find_library(ASSIMP_LIBRARY assimp PATHS "/opt/homebrew/lib" NO_DEFAULT_PATH)
    find_path(ASSIMP_INCLUDE_DIR assimp/ai_assert.h PATHS "/opt/homebrew/include" NO_DEFAULT_PATH)
    
    if(NOT GLEW_LIBRARIES AND GLEW_LIBRARY)
        set(GLEW_LIBRARIES ${GLEW_LIBRARY})
    endif()
endif()

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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio)
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio/miniaudio.h)
    execute_process(
        COMMAND curl -L "https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h" -o miniaudio.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio
        ERROR_QUIET
    )
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party/BSlogger/src ${CMAKE_CURRENT_BINARY_DIR}/third_party/stb ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio)

# RMLUI
set(VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)
set(RMLUI_VENDOR_DIR ${VENDOR_DIR}/RmlUi)
set(RMLUI_INCLUDE_DIR ${RMLUI_VENDOR_DIR}/Include)
set(RMLUI_LIB_DIR ${RMLUI_VENDOR_DIR})

# BUSCAR LIBRERÍA CORE
find_library(RMLUI_LIBRARY 
    NAMES rmlui RmlCore 
    PATHS ${RMLUI_LIB_DIR}
    NO_DEFAULT_PATH
)

# BUSCAR LIBRERÍA DEBUGGER
find_library(RMLUI_DEBUGGER_LIBRARY 
    NAMES rmlui_debugger RmlDebugger 
    PATHS ${RMLUI_LIB_DIR}
    NO_DEFAULT_PATH
)

if(NOT RMLUI_LIBRARY)
    message(FATAL_ERROR "No se pudo encontrar la librería rmlui en ${RMLUI_LIB_DIR}")
endif()

if(NOT RMLUI_DEBUGGER_LIBRARY)
    message(FATAL_ERROR "No se pudo encontrar la librería rmlui debugger en ${RMLUI_LIB_DIR}")
endif()



set(RMLUI_BACKEND_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/RmlUi_Backends)
file(MAKE_DIRECTORY ${RMLUI_BACKEND_DIR})

set(RMLUI_BACKEND_FILES
    RmlUi_Platform_GLFW.cpp
    RmlUi_Renderer_GL3.cpp
    RmlUi_Platform_GLFW.h
    RmlUi_Renderer_GL3.h
    RmlUi_Include_GL3.h
)

function(download_rmlui_backend_file filename)
    set(REPO_URL "https://raw.githubusercontent.com/mikke89/RmlUi/master/Backends")
    set(LOCAL_FILE ${RMLUI_BACKEND_DIR}/${filename})
    set(REMOTE_URL "${REPO_URL}/${filename}")
    
    if(NOT EXISTS ${LOCAL_FILE})
        message(STATUS "Descargando ${filename} desde GitHub...")
        execute_process(
            COMMAND curl -L "${REMOTE_URL}" -o "${LOCAL_FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            RESULT_VARIABLE CURL_RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(CURL_RESULT)
            message(FATAL_ERROR "Error al descargar ${filename} desde ${REMOTE_URL}")
        endif()
    endif()
    
    if(NOT EXISTS ${LOCAL_FILE})
        message(FATAL_ERROR "No se pudo descargar ${filename} en ${RMLUI_BACKEND_DIR}")
    endif()
endfunction()

foreach(BACKEND_FILE ${RMLUI_BACKEND_FILES})
    download_rmlui_backend_file(${BACKEND_FILE})
endforeach()

message(STATUS "RmlUi: Librerías estáticas desde ${RMLUI_VENDOR_DIR}")
message(STATUS "RmlUi: Archivos de backend desde ${RMLUI_BACKEND_DIR}")

# JOLT
set(VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)
find_path(JOLT_INCLUDE_DIR Jolt/Jolt.h PATHS ${VENDOR_DIR}/JoltPhysics/include)
set(JOLT_LIB_DIR ${VENDOR_DIR}/JoltPhysics/lib)

file(GLOB SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(GLS STATIC ${SRC})

target_compile_definitions(GLS PUBLIC
    JPH_PROFILE_ENABLED=1
    JPH_DEBUG_RENDERER=1
    JPH_OBJECT_STREAM=1
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(GLS PUBLIC ENGINE_DEBUG_MODE)
endif()

# Jolt fue compilado sin RTTI, pero RmlUi lo requiere
set_source_files_properties(src/HitboxRenderer.cpp PROPERTIES COMPILE_FLAGS "-fno-rtti")

target_include_directories(GLS PUBLIC 
    "${PROJECT_SOURCE_DIR}/include" 
    ${JOLT_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/third_party/miniaudio
)

find_library(JOLT_LIBRARY NAMES Jolt Jolt_d PATHS ${JOLT_LIB_DIR})
if(NOT JOLT_LIBRARY)
    message(FATAL_ERROR "No se pudo encontrar la librería Jolt en ${JOLT_LIB_DIR}")
endif()

if(APPLE)
    target_link_libraries(GLS PUBLIC 
        ${OPENGL_LIBRARY} 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY} 
        ${GLEW_LIBRARIES} 
        glfw 
        ${ASSIMP_LIBRARY} 
        ${JOLT_LIBRARY} 
        ${RMLUI_LIBRARY}
        ${RMLUI_DEBUGGER_LIBRARY}
        Freetype::Freetype
        m 
        pthread
    )
    target_include_directories(GLS PUBLIC ${ASSIMP_INCLUDE_DIR})
    if(GLEW_INCLUDE_DIR)
        target_include_directories(GLS PUBLIC ${GLEW_INCLUDE_DIR})
    endif()
else()
    target_link_libraries(GLS PUBLIC 
        GL 
        GLEW 
        glfw 
        assimp 
        ${JOLT_LIBRARY}
        ${RMLUI_DEBUGGER_LIBRARY}
        ${RMLUI_LIBRARY} 
        Freetype::Freetype
        m 
        pthread
    )
endif()



target_sources(GLS PRIVATE
    ${RMLUI_BACKEND_DIR}/RmlUi_Platform_GLFW.cpp
    ${RMLUI_BACKEND_DIR}/RmlUi_Renderer_GL3.cpp
)

target_include_directories(GLS PUBLIC 
    ${RMLUI_INCLUDE_DIR}
    ${RMLUI_BACKEND_DIR}
    #${FREETYPE_INCLUDE_DIRS}
)


if(UNIX)
    target_link_libraries(GLS PUBLIC ${CMAKE_DL_LIBS})
endif()

# EJECUTABLE
add_executable(GLSX main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(GLSX PRIVATE ENGINE_DEBUG_MODE)
endif()

target_link_libraries(GLSX PRIVATE GLS)