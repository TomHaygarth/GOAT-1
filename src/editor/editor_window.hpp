#pragma once

#include <memory>

#include "window/window.hpp"

#include "renderer/renderable.hpp"

class EditorWindow : public Window::IWindowFunctions
{
public:

    EditorWindow();
    virtual ~EditorWindow();

    virtual void Input() override;
    virtual void Render(Renderer::IRenderContext * render_context) override;

    virtual bool WindowShouldClose() override;

private:
    bool m_should_close;
    bool m_display_about;
    bool m_display_imgui_demo;

    std::unique_ptr<Renderer::IRenderable> m_test_renderable = nullptr;
};
