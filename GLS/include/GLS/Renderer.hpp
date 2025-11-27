#include "Shader.hpp"
#include "Scene.hpp"
#include "HitboxRenderer.hpp"
#include <Jolt/Physics/Body/BodyManager.h>

#ifndef RENDER_HPP
#define RENDER_HPP

// Forward declaration para evitar dependencia circular
class UIManager;

namespace Engine
{

class Renderer
{
public:
    Renderer();

    Renderer(const Renderer& render) = delete;

    Renderer(Renderer&& render) = delete;

    ~Renderer();

    Renderer& operator = (const Renderer& render) = delete;

    Renderer& operator = (Renderer&& render) = delete;

    void debug();

    void init();

    void render(std::shared_ptr<Scene> scene);

    void stop();

    void RenderDebug(std::shared_ptr<CameraComponent> camera);

    void clear() noexcept;

    GLfloat getDeltaTime() const noexcept;

    // UI Manager integration (opcional)
    void setUIManager(std::shared_ptr<::UIManager> ui_manager) noexcept;

private:
    std::vector<std::shared_ptr<Shader>> shaders;
    bool running;
    std::shared_ptr<Shader> currentShader;
    Shader::LIST activeShade;
    std::shared_ptr<Engine::HitboxRenderer> hitboxRenderer;
    #ifdef JPH_DEBUG_RENDERER
        JPH::BodyManager::DrawSettings drawSetting;
    #endif

    void renderDirLight(std::shared_ptr<DirectionalLight> dirLight);
    void renderCamera(std::shared_ptr<CameraComponent> camera);
    void renderObject(std::shared_ptr<GameObject> object);
    void renderModel(std::shared_ptr<ModelComponent> model, glm::mat4 m);
    void renderSkyBox(std::shared_ptr<SkyBox> sky_box,std::shared_ptr<CameraComponent> camera);

    void useShader(Shader::LIST shader);

    void calcDeltaTime();

    GLdouble last_frame_time{0.0}; // Tiempo al final del frame anterior (usamos GLdouble por la precisión de glfwGetTime)
    GLfloat delta_time{0.f};       // Almacenamos el delta time en float para su uso en OpenGL/física

    // UI Manager (opcional)
    std::shared_ptr<::UIManager> ui_manager{nullptr};
};
  
}

#endif // RENDER_HPP