#include <array>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef INPUT_HPP
#define INPUT_HPP

//#define DEBUG 1

struct MousePair
{
    double x;
    double y;
};


class Input : public std::enable_shared_from_this<Input>
{
public:
    Input() = default;

    Input(const Input& input) = delete;

    Input(Input&& input) = delete;

    virtual ~Input() = default;

    Input& operator = (const Input& input) = delete;

    Input& operator = (Input&& input) = delete;

    static std::shared_ptr<Input> create();

    std::shared_ptr<Input> self();

    void init(GLFWwindow* window) noexcept;

    bool is_key_pressed(int key) const noexcept;

    bool is_mouse_button_pressed(int button) const noexcept;

    MousePair get_mouse_position() const noexcept;

    MousePair get_mouse_delta() const noexcept;

    virtual void update(const float &dt) noexcept {};

    void poll(const float &dt) noexcept;

private:
    std::array<bool, GLFW_KEY_LAST + 1> keys {};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> mouse_buttons {};
    double mouse_x {0.f};
    double mouse_y {0.f};
    double x_change {0.f};
    double y_change {0.f};

    static void handle_keyboard_input(GLFWwindow* window, int key, int code, int action, int mode) noexcept;

    static void handle_mouse_input(GLFWwindow* window, double x_pos, double y_pos) noexcept;

    static void handle_mouse_button_input(GLFWwindow* window, int button, int action, int mods) noexcept;
};  

#endif // INPUT_HPP