#include "Shader.hpp"
#include "Scene.hpp"

#ifndef RENDER_HPP
#define RENDER_HPP


class Renderer
{
public:
    Renderer();

    Renderer(const Renderer& render) = delete;

    Renderer(Renderer&& render) = delete;

    ~Renderer();

    Renderer& operator = (const Renderer& render) = delete;

    Renderer& operator = (Renderer&& render) = delete;

    void init();

    void render(std::shared_ptr<Scene> scene);

    void stop();

    void clear() noexcept;

    GLfloat getDeltaTime() const noexcept;

   

private:
    std::vector<std::shared_ptr<Shader>> shaders;
    bool running;
    std::shared_ptr<Shader> currentShader;

    void renderDirLight(std::shared_ptr<DirectionalLight> dirLight);
    void renderCamera(std::shared_ptr<CameraComponent> camera);
    void renderObject(std::shared_ptr<GameObject> object, std::shared_ptr<Scene> scene);
    void renderModel(std::shared_ptr<ModelComponent> model, std::shared_ptr<Scene> scene);
    void renderSkyBox(std::shared_ptr<SkyBox> sky_box,std::shared_ptr<CameraComponent> camera);

    void calcDeltaTime();

    GLdouble last_frame_time{0.0}; // Tiempo al final del frame anterior (usamos GLdouble por la precisión de glfwGetTime)
    GLfloat delta_time{0.f};       // Almacenamos el delta time en float para su uso en OpenGL/física

};  

#endif // RENDER_HPP