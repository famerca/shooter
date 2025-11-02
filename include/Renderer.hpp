#include "Shader.hpp"
#include "Scene.hpp"

#ifndef RENDER_HPP
#define RENDER_HPP


class Renderer
{
public:
    Render();

    Render(const Render& render) = delete;

    Render(Render&& render) = delete;

    ~Render();

    Render& operator = (const Render& render) = delete;

    Render& operator = (Render&& render) = delete;

    void init();

    void start(std::shared_ptr<Scene> scene);

    void stop();

    void clear();

private:

    void clear() noexcept;
    std::vector<std::shared_ptr<Shader>> shaders;
    bool running;
    
};  

#endif // RENDER_HPP