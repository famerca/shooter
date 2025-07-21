#include "Window.h"

// Variables estáticas para la gestión de teclas (si quieres añadir más adelante)
// bool* keys;

Window::Window(GLint windowWidth, GLint windowHeight, const char* windowTitle)
    : width(windowWidth), height(windowHeight), title(windowTitle), mainWindow(nullptr), bufferWidth(0), bufferHeight(0)
{
    // Inicializar todas las teclas a false
    for (int i = 0; i < 1024; i++)
        keys[i] = false;
}

int Window::Init()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed!\n";
        glfwTerminate();
        return 1; // Return non-zero for error
    }

    // Setup GLFW window properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    // No backward compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!mainWindow)
    {
        std::cerr << "GLFW window creation failed!\n";
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Get buffer size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW
    glfwMakeContextCurrent(mainWindow);

    // Handle key and mouse input here (optional, for later use)
    CreateCallbacks();

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Glew intialization failed!\n";
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    return 0; // Success
}

Window::~Window()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(mainWindow);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(mainWindow);
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::CreateCallbacks()
{
    // Set the user pointer for the window so we can access Window instance in static callback
    glfwSetWindowUserPointer(mainWindow, this);
    glfwSetKeyCallback(mainWindow, HandleKeys);
}

void Window::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            theWindow->keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            theWindow->keys[key] = false;
        }
    }
}