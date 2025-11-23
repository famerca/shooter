#include <Input.hpp>
#include <Window.hpp>

namespace Engine
{

std::shared_ptr<Input> Input::create()
{
    #ifdef DEBUG
        LOG_INIT_CERR();
    #endif
    return std::make_shared<Input>();
}

std::shared_ptr<Input> Input::self()
{
    return shared_from_this();
}

void Input::init(GLFWwindow* window) noexcept
{
    glfwSetKeyCallback(window, Input::handle_keyboard_input);
    glfwSetCursorPosCallback(window, Input::handle_mouse_input);
    glfwSetMouseButtonCallback(window, Input::handle_mouse_button_input);
}

bool Input::is_key_pressed(int key) const noexcept
{
    return keys[key];
}

bool Input::is_mouse_button_pressed(int button) const noexcept
{
    return mouse_buttons[button];
}

MousePair Input::get_mouse_position() const noexcept
{
    return {mouse_x, mouse_y};
}

MousePair Input::get_mouse_delta() const noexcept
{
    return {x_change, -y_change};
}

void Input::handle_keyboard_input(GLFWwindow* window, int key, int code, int action, int mode) noexcept
{
    #ifdef DEBUG
        LOG_INIT_COUT();
    #endif
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    std::shared_ptr<Input> input = win->getInput();

    if (input)
    {
        if (action == GLFW_PRESS)
        {
            input->keys[key] = true;
            #ifdef DEBUG
                log(LOG_INFO) << "Key pressed: " << key << "\n";
            #endif
        }
        else if (action == GLFW_RELEASE)
        {
            input->keys[key] = false;
            #ifdef DEBUG
                log(LOG_INFO) << "Key released: " << key << "\n";
            #endif
        }
    }


}

void Input::handle_mouse_input(GLFWwindow* window, double x_pos, double y_pos) noexcept
{
    #ifdef DEBUG
        LOG_INIT_COUT();
    #endif
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    std::shared_ptr<Input> input = win->getInput();

    if (input)
    {
        // 1. Calcular el desplazamiento desde la ÚLTIMA POSICIÓN REGISTRADA (input->mouse_x/y)
        double dx = x_pos - input->mouse_x;
        double dy = y_pos - input->mouse_y;

        // 2. ¡ACUMULAR! Sumar este desplazamiento al delta total del frame
        input->x_change += dx; 
        input->y_change += dy; 
        
        // 3. Actualizar la última posición del ratón para el PRÓXIMO evento de callback
        input->mouse_x = x_pos;
        input->mouse_y = y_pos;
    }

    #ifdef DEBUG
        log(LOG_INFO) << "x: " << x_pos << ", y: " << y_pos << "\ndx: " << input->x_change << ", dy: " << input->y_change << "\n";
    #endif
}

// En Input.cpp

void Input::handle_mouse_button_input(GLFWwindow* window, int button, int action, int mods) noexcept
{
    // 1. Obtener el puntero a la instancia de Window
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    
    // 2. Obtener el shared_ptr a la instancia de Input
    std::shared_ptr<Input> input = win ? win->getInput() : nullptr;

    if (input)
    {
        // 3. Verificar que el botón esté dentro de los límites del array
        if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST)
        {
            if (action == GLFW_PRESS)
            {
                // El botón ha sido presionado
                input->mouse_buttons[button] = true;
                
                // Opcional: Llamada al callback virtual on_mouse_down()
                // input->on_mouse_down(button);
            }
            else if (action == GLFW_RELEASE)
            {
                // El botón ha sido liberado
                input->mouse_buttons[button] = false;
                
                // Opcional: Llamada al callback virtual on_mouse_up()
                // input->on_mouse_up(button);
            }
        }
    }
}

void Input::poll(const float &dt) noexcept
{

    this->update(dt);

    x_change = 0.f;
    y_change = 0.f;
}

}