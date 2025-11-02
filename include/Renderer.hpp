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

    void start(std::shared_ptr<Scene> scene);

    void stop();

    void clear();

private:

    void clear() noexcept;
    std::vector<std::shared_ptr<Shader>> shaders;
    bool running;
    std::shared_ptr<Shader> currentShader;
    void render(std::shared_ptr<DirectionalLight> DirLight);
    void render(std::shared_ptr<CameraComponent> camera);
    
};  

#endif // RENDER_HPP