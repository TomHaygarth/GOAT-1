#pragma once

#include "window/window.hpp"

class EditorWindow : public Window::IWindowFunctions
{
public:

    EditorWindow();
    virtual ~EditorWindow();

    virtual void Input() override;
    virtual void Render() override;

    virtual bool WindowShouldClose() override;

private:
    bool m_should_close;
    bool m_display_about;
    bool m_display_imgui_demo;
};
