cmake_minimum_required(VERSION 3.20)

# Project name
project(GLShooter)

# Set C++ Required version
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

# Set compiler flags (platform-specific)
if(MSVC)
    # MSVC compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc")
    # Define __PRETTY_FUNCTION__ for MSVC (used by BSlogger)
    add_definitions(-D__PRETTY_FUNCTION__=__FUNCSIG__)
elseif(UNIX AND NOT APPLE)
    # Linux compiler flags
    set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings -Wno-parentheses -Wfloat-equal -pedantic ${CMAKE_CXX_FLAGS}")
elseif(APPLE)
    # macOS compiler flags
    set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings -Wno-parentheses -Wfloat-equal -pedantic ${CMAKE_CXX_FLAGS}")
elseif(MINGW)
    # MinGW compiler flags
    set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings -Wno-parentheses -Wfloat-equal -pedantic ${CMAKE_CXX_FLAGS}")
endif()

# Use -std=c++XX instead of -std=gnu++XX
set(CMAKE_CXX_EXTENSIONS OFF)

# Set dependencies
find_package(OpenGL REQUIRED)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Find GLM (header-only library)
find_package(glm QUIET CONFIG)
if(NOT glm_FOUND)
    # Try to find GLM manually or via vcpkg
    if(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
        get_filename_component(VCPKG_ROOT "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
        get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
        get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
        set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/x64-windows")
        find_path(GLM_INCLUDE_DIR 
            NAMES glm/glm.hpp
            PATHS
            ${VCPKG_INSTALLED_DIR}/include
            ${CMAKE_SOURCE_DIR}/third_party/glm
            ENV GLM_ROOT_DIR
            PATH_SUFFIXES include
        )
        if(GLM_INCLUDE_DIR)
            set(glm_FOUND TRUE)
            message(STATUS "Found GLM manually: ${GLM_INCLUDE_DIR}")
        endif()
    endif()
    
    # Try to find GLM in common locations
    if(NOT glm_FOUND)
        find_path(GLM_INCLUDE_DIR 
            NAMES glm/glm.hpp
            PATHS
            ${CMAKE_SOURCE_DIR}/third_party/glm
            ${CMAKE_SOURCE_DIR}/../third_party/glm
            "C:/Program Files/glm/include"
            "C:/glm/include"
            ENV GLM_ROOT_DIR
            PATH_SUFFIXES include
        )
        if(GLM_INCLUDE_DIR)
            set(glm_FOUND TRUE)
            message(STATUS "Found GLM manually: ${GLM_INCLUDE_DIR}")
        endif()
    endif()
    
    # Download GLM using FetchContent if not found (header-only, easy to download)
    if(NOT glm_FOUND)
        message(STATUS "GLM not found, downloading from GitHub...")
        FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG        0.9.9.8
        )
        FetchContent_GetProperties(glm)
        if(NOT glm_POPULATED)
            FetchContent_Populate(glm)
            set(GLM_INCLUDE_DIR ${glm_SOURCE_DIR})
            set(glm_FOUND TRUE)
            message(STATUS "GLM downloaded to: ${GLM_INCLUDE_DIR}")
        endif()
    endif()
endif()

# Try to use vcpkg if available (common on Windows)
if(WIN32)
    # Check for vcpkg toolchain
    if(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
        message(STATUS "Using vcpkg toolchain")
    endif()
endif()

# Find or download GLEW
if(WIN32)
    # Try to find GLEW - vcpkg provides it via find_package
    find_package(GLEW QUIET)
    if(NOT GLEW_FOUND)
        # Try CONFIG mode explicitly
        find_package(GLEW QUIET CONFIG)
    endif()
    # If still not found and using vcpkg, try to find it manually in vcpkg paths
    if(NOT GLEW_FOUND AND EXISTS "${CMAKE_TOOLCHAIN_FILE}")
        # Extract vcpkg root from toolchain file path
        get_filename_component(VCPKG_ROOT "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
        get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
        get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
        set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/x64-windows")
        
        # Set CMAKE_PREFIX_PATH to help find_package
        list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALLED_DIR}")
        find_package(GLEW QUIET)
    endif()
    if(NOT GLEW_FOUND)
        # Try to find GLEW in vcpkg installation
        if(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
            # Extract vcpkg root from toolchain file path
            # Toolchain file is at: C:/vcpkg/scripts/buildsystems/vcpkg.cmake
            # So we need to go up 2 levels to get to C:/vcpkg
            get_filename_component(VCPKG_ROOT "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
            get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
            get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
            set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/x64-windows")
            
            find_path(GLEW_INCLUDE_DIRS 
                NAMES GL/glew.h
                PATHS
                ${VCPKG_INSTALLED_DIR}/include
                ${CMAKE_SOURCE_DIR}/third_party/glew/include
                ${CMAKE_SOURCE_DIR}/../third_party/glew/include
                "C:/Program Files/glew/include"
                "C:/glew/include"
                ENV GLEW_ROOT_DIR
                PATH_SUFFIXES include
            )
            find_library(GLEW_LIBRARIES
                NAMES glew32s glew32 libglew32s libglew32 GLEW glew glew32mx glew32mxs
                PATHS
                ${VCPKG_INSTALLED_DIR}/lib
                ${VCPKG_INSTALLED_DIR}/debug/lib
                ${CMAKE_SOURCE_DIR}/third_party/glew/lib
                ${CMAKE_SOURCE_DIR}/../third_party/glew/lib
                "C:/Program Files/glew/lib"
                "C:/glew/lib"
                ENV GLEW_ROOT_DIR
                PATH_SUFFIXES lib lib/Release/x64 lib/Release/Win32
            )
            # Prefer static library if available
            find_library(GLEW_LIBRARIES_STATIC
                NAMES glew32s libglew32s
                PATHS
                ${VCPKG_INSTALLED_DIR}/lib
                ${VCPKG_INSTALLED_DIR}/debug/lib
                NO_DEFAULT_PATH
            )
            if(GLEW_LIBRARIES_STATIC)
                set(GLEW_LIBRARIES ${GLEW_LIBRARIES_STATIC})
            endif()
        else()
            # Try to find GLEW manually
            find_path(GLEW_INCLUDE_DIRS 
                NAMES GL/glew.h
                PATHS
                ${CMAKE_SOURCE_DIR}/third_party/glew/include
                ${CMAKE_SOURCE_DIR}/../third_party/glew/include
                "C:/Program Files/glew/include"
                "C:/glew/include"
                ENV GLEW_ROOT_DIR
                PATH_SUFFIXES include
            )
            find_library(GLEW_LIBRARIES
                NAMES glew32s glew32 libglew32s libglew32 GLEW
                PATHS
                ${CMAKE_SOURCE_DIR}/third_party/glew/lib
                ${CMAKE_SOURCE_DIR}/../third_party/glew/lib
                "C:/Program Files/glew/lib"
                "C:/glew/lib"
                ENV GLEW_ROOT_DIR
                PATH_SUFFIXES lib lib/Release/x64 lib/Release/Win32
            )
        endif()
        if(GLEW_INCLUDE_DIRS AND GLEW_LIBRARIES)
            set(GLEW_FOUND TRUE)
            message(STATUS "Found GLEW manually: ${GLEW_LIBRARIES}")
            message(STATUS "GLEW include dir: ${GLEW_INCLUDE_DIRS}")
        else()
            message(STATUS "DEBUG: GLEW_INCLUDE_DIRS = ${GLEW_INCLUDE_DIRS}")
            message(STATUS "DEBUG: GLEW_LIBRARIES = ${GLEW_LIBRARIES}")
            if(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
                message(STATUS "DEBUG: VCPKG_INSTALLED_DIR = ${VCPKG_INSTALLED_DIR}")
            endif()
        endif()
    endif()
    if(NOT GLEW_FOUND)
        message(STATUS "")
        message(STATUS "==========================================")
        message(STATUS "GLEW not found!")
        message(STATUS "Please install GLEW using one of these methods:")
        message(STATUS "1. Using vcpkg (RECOMMENDED):")
        message(STATUS "   vcpkg install glew:x64-windows")
        message(STATUS "   Then run: cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "")
        message(STATUS "2. Download pre-built from:")
        message(STATUS "   https://github.com/nigels-com/glew/releases")
        message(STATUS "   Extract to: ${CMAKE_SOURCE_DIR}/third_party/glew/")
        message(STATUS "   Structure: third_party/glew/include/GL/glew.h")
        message(STATUS "              third_party/glew/lib/Release/x64/glew32s.lib")
        message(STATUS "")
        message(STATUS "3. Or set GLEW_ROOT_DIR environment variable")
        message(STATUS "==========================================")
        message(STATUS "")
        message(FATAL_ERROR "GLEW is required. Please install it and try again.")
    endif()
else()
    find_package(GLEW REQUIRED)
endif()

# Find or download GLFW
if(WIN32)
    find_package(glfw3 QUIET)
    if(NOT glfw3_FOUND)
        # Try to find GLFW manually first
        find_path(GLFW_INCLUDE_DIR
            NAMES GLFW/glfw3.h
            PATHS
            ${CMAKE_SOURCE_DIR}/third_party/glfw/include
            ${CMAKE_SOURCE_DIR}/../third_party/glfw/include
            "C:/Program Files/glfw/include"
            "C:/glfw/include"
            ENV GLFW_ROOT_DIR
            PATH_SUFFIXES include
        )
        find_library(GLFW_LIBRARY
            NAMES glfw3 glfw libglfw3 libglfw
            PATHS
            ${CMAKE_SOURCE_DIR}/third_party/glfw/lib
            ${CMAKE_SOURCE_DIR}/../third_party/glfw/lib
            "C:/Program Files/glfw/lib"
            "C:/glfw/lib"
            ENV GLFW_ROOT_DIR
            PATH_SUFFIXES lib lib-vc2015 lib-vc2017 lib-vc2019 lib-vc2022
        )
        if(GLFW_INCLUDE_DIR AND GLFW_LIBRARY)
            set(glfw3_FOUND TRUE)
            set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
            set(GLFW_LIBRARIES ${GLFW_LIBRARY})
            message(STATUS "Found GLFW manually: ${GLFW_LIBRARY}")
        endif()
    endif()
    if(NOT glfw3_FOUND)
        # Download and build GLFW using FetchContent
        message(STATUS "GLFW not found, downloading and building from source...")
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        3.3.8
        )
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(glfw)
        set(glfw3_FOUND TRUE)
        set(GLFW_INCLUDE_DIRS ${glfw_SOURCE_DIR}/include)
        set(GLFW_LIBRARIES glfw)
    endif()
else()
    find_package(glfw3 REQUIRED)
endif()

# Platform-specific dependency handling
if(APPLE)
    # macOS specific: Find OpenGL framework
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
elseif(WIN32)
    # Windows: Find Assimp
    find_package(assimp QUIET CONFIG)
    if(NOT assimp_FOUND)
        find_package(assimp QUIET)
    endif()
    if(NOT assimp_FOUND)
        # Try to find assimp in vcpkg installation
        if(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
            get_filename_component(VCPKG_ROOT "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
            get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
            get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
            set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/x64-windows")
            
            find_library(ASSIMP_LIBRARY 
                NAMES assimp-vc143-mt assimp-vc142-mt assimp assimp-vc141-mt
                PATHS
                ${VCPKG_INSTALLED_DIR}/lib
                ${VCPKG_INSTALLED_DIR}/debug/lib
                ${CMAKE_SOURCE_DIR}/lib
                ${CMAKE_SOURCE_DIR}/../lib
                ${CMAKE_SOURCE_DIR}/../../lib
                ENV ASSIMP_ROOT_DIR
                PATH_SUFFIXES lib lib64
            )
            find_path(ASSIMP_INCLUDE_DIR 
                NAMES assimp/ai_assert.h
                PATHS
                ${VCPKG_INSTALLED_DIR}/include
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_SOURCE_DIR}/../include
                ${CMAKE_SOURCE_DIR}/../../include
                ENV ASSIMP_ROOT_DIR
                PATH_SUFFIXES include
            )
            if(ASSIMP_LIBRARY AND ASSIMP_INCLUDE_DIR)
                set(assimp_FOUND TRUE)
                message(STATUS "Found Assimp manually: ${ASSIMP_LIBRARY}")
            endif()
        else()
            # Try to find assimp manually on Windows
            find_library(ASSIMP_LIBRARY 
                NAMES assimp-vc143-mt assimp-vc142-mt assimp assimp-vc141-mt
                PATHS
                ${CMAKE_SOURCE_DIR}/lib
                ${CMAKE_SOURCE_DIR}/../lib
                ${CMAKE_SOURCE_DIR}/../../lib
                ENV ASSIMP_ROOT_DIR
                PATH_SUFFIXES lib lib64
            )
            find_path(ASSIMP_INCLUDE_DIR 
                NAMES assimp/ai_assert.h
                PATHS
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_SOURCE_DIR}/../include
                ${CMAKE_SOURCE_DIR}/../../include
                ENV ASSIMP_ROOT_DIR
                PATH_SUFFIXES include
            )
        endif()
    endif()
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)
execute_process(
    COMMAND git clone "https://github.com/bschiffthaler/BSlogger.git" BSlogger
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)
execute_process(
    COMMAND git clone "https://github.com/nothings/stb.git" stb
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party/BSlogger/src ${CMAKE_CURRENT_BINARY_DIR}/third_party/stb)
if(GLM_INCLUDE_DIR)
    include_directories(${GLM_INCLUDE_DIR})
endif()

file(GLOB SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(GLSHOOTERLIB ${SRC})
target_include_directories(GLSHOOTERLIB PUBLIC "${PROJECT_SOURCE_DIR}/include")


if(APPLE)
    target_link_libraries(GLSHOOTERLIB PUBLIC ${OPENGL_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY} ${GLEW_LIBRARIES} glfw ${ASSIMP_LIBRARY})
    target_include_directories(GLSHOOTERLIB PUBLIC ${ASSIMP_INCLUDE_DIR})
elseif(WIN32)
    # Windows: Link OpenGL, GLEW, GLFW, and Assimp
    target_link_libraries(GLSHOOTERLIB PUBLIC opengl32)
    
    # Link GLEW
    if(TARGET GLEW::GLEW)
        target_link_libraries(GLSHOOTERLIB PUBLIC GLEW::GLEW)
    else()
        target_include_directories(GLSHOOTERLIB PUBLIC ${GLEW_INCLUDE_DIRS})
        # Define GLEW_STATIC if using static library
        if(GLEW_LIBRARIES MATCHES "glew32s")
            target_compile_definitions(GLSHOOTERLIB PUBLIC GLEW_STATIC)
        endif()
        target_link_libraries(GLSHOOTERLIB PUBLIC ${GLEW_LIBRARIES})
    endif()
    
    # Link GLFW
    if(TARGET glfw)
        target_link_libraries(GLSHOOTERLIB PUBLIC glfw)
    elseif(TARGET glfw3)
        target_link_libraries(GLSHOOTERLIB PUBLIC glfw3)
    else()
        target_include_directories(GLSHOOTERLIB PUBLIC ${GLFW_INCLUDE_DIRS})
        target_link_libraries(GLSHOOTERLIB PUBLIC ${GLFW_LIBRARIES})
    endif()
    
    # Link Assimp
    if(ASSIMP_LIBRARY)
        target_link_libraries(GLSHOOTERLIB PRIVATE ${ASSIMP_LIBRARY})
        if(ASSIMP_INCLUDE_DIR)
            target_include_directories(GLSHOOTERLIB PUBLIC ${ASSIMP_INCLUDE_DIR})
        endif()
    else()
        # Try to find and link assimp via find_package
        find_package(assimp QUIET)
        if(assimp_FOUND)
            if(TARGET assimp::assimp)
                target_link_libraries(GLSHOOTERLIB PRIVATE assimp::assimp)
            else()
                target_link_libraries(GLSHOOTERLIB PRIVATE assimp)
            endif()
        else()
            message(WARNING "Assimp not found. You may need to install it or set ASSIMP_ROOT_DIR")
            target_link_libraries(GLSHOOTERLIB PRIVATE assimp)
        endif()
    endif()
else()
    # Linux and other Unix-like systems
    target_link_libraries(GLSHOOTERLIB PUBLIC GL GLEW glfw)
    target_link_libraries(GLSHOOTERLIB PRIVATE assimp)
endif()

# Set the main source to generate the executable code
add_executable(GLShooter main.cpp)

target_link_libraries(GLShooter GLSHOOTERLIB)

# Copy DLLs to output directory on Windows
if(WIN32 AND EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    get_filename_component(VCPKG_ROOT "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
    get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
    get_filename_component(VCPKG_ROOT "${VCPKG_ROOT}" DIRECTORY)
    set(VCPKG_BIN_DIR "${VCPKG_ROOT}/installed/x64-windows/bin")
    
    add_custom_command(TARGET GLShooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_BIN_DIR}/glew32.dll"
        "${VCPKG_BIN_DIR}/glfw3.dll"
        "${VCPKG_BIN_DIR}/assimp-vc143-mt.dll"
        $<TARGET_FILE_DIR:GLShooter>
        COMMENT "Copying DLLs to output directory"
    )
endif()
