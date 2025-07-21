#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream> // Para std::cerr

class Window
{
public:
    Window(GLint windowWidth, GLint windowHeight, const char* windowTitle);
    ~Window();

    int Init();
    bool ShouldClose();
    void SwapBuffers();
    void PollEvents();

    GLint GetBufferWidth() { return bufferWidth; }
    GLint GetBufferHeight() { return bufferHeight; }

    // Nuevo: método para acceder al array de teclas
    bool* GetKeys() { return keys; }

private:
    GLFWwindow* mainWindow;
    GLint width, height;
    int bufferWidth, bufferHeight;
    const char* title;

    // Nuevo: array de teclas
    bool keys[1024];

    void CreateCallbacks();
    static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
};