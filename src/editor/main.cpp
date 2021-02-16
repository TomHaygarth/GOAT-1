#include "editor_window.hpp"
#include "window/window.hpp"

#include <memory>
#include <utility>

int main ()
{
    EditorWindow * editor_window_funcs =  new EditorWindow();

    Window::WindowInstance * window_ptr = Window::create_window("OGAT - GOAT Editor",
                                                                1440,
                                                                900,
                                                                std::move(editor_window_funcs));
    int exit_code = window_ptr->Run();

    Window::destroy_window(window_ptr);

    delete editor_window_funcs;
    editor_window_funcs = nullptr;
    return exit_code;
}
