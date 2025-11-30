#ifndef GAME_HPP
#define GAME_HPP
#include <GL/glew.h>
#include <GLS/Window.hpp>
#include <GLS/Scene.hpp>
#include <GLS/Renderer.hpp>
#include <GLS/GameObject.hpp>
#include <GLS/Listener.hpp>
#include <GLS/UIManager.hpp>


class inputManager;

class Game
{
public:
    Game(std::shared_ptr<Engine::Window> window);
    ~Game();

    void init();

    void Level1();

    void render();

    // Función para reiniciar el juego (llamada desde UI)
    void resetGame() noexcept;

private:
    std::shared_ptr<Engine::Window> m_window;
    std::shared_ptr<Engine::Scene> m_scene;
    std::shared_ptr<Engine::Renderer> m_renderer;
    std::shared_ptr<inputManager> m_input;
    std::shared_ptr<Engine::GameObject> m_user;
    std::shared_ptr<Engine::CameraComponent> m_camera;
    std::shared_ptr<UIManager> m_ui_manager;

    Engine:: Listener::Callback ground_collition;

    unsigned m_user_index;


    void handleGameOver() noexcept;

    void initScene();
    void initSkyBox();
    void initUser();
    void initInput();
    void initRenderer();
    void initGround();
    void initCollitions();
    void initUI();
};


#endif // GAME_HPP