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

private:
    GLFWwindow* mainWindow;
    GLint width, height;
    int bufferWidth, bufferHeight;
    const char* title;

    void CreateCallbacks();
    static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
};