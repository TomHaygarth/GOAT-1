#include "window/window.hpp"

int main ()
{
    Window::WindowInstance * window_ptr = Window::create_window("OGAT - GOAT", 800, 600, nullptr);
    int exit_code = window_ptr->Run();

    Window::destroy_window(window_ptr);
    return exit_code;
}
