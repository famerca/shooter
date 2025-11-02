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

private:
    std::vector<std::shared_ptr<Shader>> shaders;
    bool running;
    std::shared_ptr<Shader> currentShader;
    void renderDirLight(std::shared_ptr<DirectionalLight> dirLight);
    void renderCamera(std::shared_ptr<CameraComponent> camera);
    void renderObject(std::shared_ptr<GameObject> object);
    void renderModel(std::shared_ptr<ModelComponent> model);


    
};  

#endif // RENDER_HPP