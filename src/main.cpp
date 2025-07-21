#include "Window.h"
#include "Shader.h"
#include "Mesh.h"

// Define paths to your shader files
const char* vertexShaderPath = "../shaders/basic.vert.glsl";
const char* fragmentShaderPath = "../shaders/basic.frag.glsl";

int main()
{
    // Window dimensions
    constexpr GLint WIDTH = 800;
    constexpr GLint HEIGHT = 600;
    const char* TITLE = "OG Shooter";

    // Create Window object
    Window mainWindow(WIDTH, HEIGHT, TITLE);
    if (mainWindow.Init() != 0)
    {
        return EXIT_FAILURE;
    }

    // Variables para el desplazamiento del triángulo
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    const float moveSpeed = 0.02f;

    // Define vertices for the triangle (se actualizarán en cada frame)
    GLfloat vertices[] = {
        -1.f, -1.f, 0.f,
        1.f, -1.f, 0.f,
        0.f, 1.f, 0.f
    };

    // Create Mesh object
    Mesh triangleMesh;
    triangleMesh.CreateMesh(vertices, 9); // 9 because 3 vertices * 3 components (x,y,z)

    // Create Shader object
    Shader basicShader;
    basicShader.CreateFromFiles(vertexShaderPath, fragmentShaderPath);

    // Main loop
    while (!mainWindow.ShouldClose())
    {
        // Get and handle user input events
        mainWindow.PollEvents();

        // Leer teclas
        bool* keys = mainWindow.GetKeys();
        if (keys[GLFW_KEY_LEFT])  xOffset -= moveSpeed;
        if (keys[GLFW_KEY_RIGHT]) xOffset += moveSpeed;
        if (keys[GLFW_KEY_UP])    yOffset += moveSpeed;
        if (keys[GLFW_KEY_DOWN])  yOffset -= moveSpeed;

        // Actualizar los vértices con el desplazamiento
        GLfloat movedVertices[] = {
            -1.f + xOffset, -1.f + yOffset, 0.f,
             1.f + xOffset, -1.f + yOffset, 0.f,
             0.f + xOffset,  1.f + yOffset, 0.f
        };
        triangleMesh.CreateMesh(movedVertices, 9);

        // Clear the window
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        basicShader.UseShader();
        
        // Render the mesh
        triangleMesh.RenderMesh();
        
        // Unuse shader program (good practice)
        glUseProgram(0);

        // Swap buffers to display the rendered frame
        mainWindow.SwapBuffers();
    }
    
    // Cleanup (destructors will handle most of it)
    // You could explicitly call ClearMesh() and ClearShader() if you wanted
    // but the destructors of `triangleMesh` and `basicShader` will do it automatically
    // when they go out of scope.

    return EXIT_SUCCESS;
}