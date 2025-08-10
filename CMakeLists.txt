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
find_package(assimp REQUIRED)

# macOS specific: Find OpenGL framework
if(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    
    # Set GLEW paths for macOS
    set(GLEW_INCLUDE_DIRS "/opt/homebrew/include")
    set(GLEW_LIBRARIES "/opt/homebrew/lib/libGLEW.dylib")
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)
execute_process(
    COMMAND git clone "https://github.com/bschiffthaler/BSlogger.git" BSlogger
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/third_party)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party/BSlogger/src)

file(GLOB SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(GLSHOOTERLIB ${SRC})
target_include_directories(GLSHOOTERLIB PUBLIC "${PROJECT_SOURCE_DIR}/include")


if(APPLE)
    target_link_libraries(GLSHOOTERLIB PUBLIC ${OPENGL_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY} ${GLEW_LIBRARIES} glfw)
else()
    target_link_libraries(GLSHOOTERLIB PUBLIC GL GLEW glfw )
    target_link_libraries(GLSHOOTERLIB PRIVATE assimp)
endif()

# Set the main source to generate the executable code
add_executable(GLShooter main.cpp)


target_link_libraries(GLShooter GLSHOOTERLIB)
