cmake_minimum_required(VERSION 3.20)

# Project name
project(OGShooter)

# Set C++ Required version
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)

# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings -Wno-parentheses -Wfloat-equal -pedantic ${CMAKE_CXX_FLAGS}")

# Use -std=c++XX instead of -std=gnu++XX
set(CMAKE_CXX_EXTENSIONS OFF)

# Set dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Source files (including main.cpp)
file(GLOB SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Split main.cpp from the rest
list(FILTER SRC EXCLUDE REGEX ".*/main\\.cpp$")

# Create library from source files (excluding main)
add_library(OGShooterLib ${SRC})
target_include_directories(OGShooterLib PUBLIC "${PROJECT_SOURCE_DIR}/include")

# 👉 Enlazar las dependencias a la librería, porque ahí se usan
target_link_libraries(OGShooterLib PUBLIC GL GLEW glfw)

# Ejecutable usando main.cpp
add_executable(OGShooter "${PROJECT_SOURCE_DIR}/src/main.cpp")

# Enlazar ejecutable con la librería
target_link_libraries(OGShooter OGShooterLib)
