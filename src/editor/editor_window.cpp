#include "editor_window.hpp"

#include "imgui.h"

#include "renderer/primitives/shape_2d.hpp"

EditorWindow::EditorWindow()
: m_should_close(false)
, m_display_about(false)
, m_display_imgui_demo(false)
{
}


EditorWindow::~EditorWindow()
{
}

void EditorWindow::Input()
{
    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New..."))
        {
            // TODO:
        }
        
        if (ImGui::MenuItem("Save"))
        {
            // TODO:
        }
        
        if (ImGui::MenuItem("Save Project"))
        {
            // TODO:
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Exit"))
        {
            m_should_close = true;
        }

        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Testing"))
    {
        bool test_triangle_enabled = m_test_renderable != nullptr;

        if (ImGui::MenuItem("Draw Triangle", "", &test_triangle_enabled))
        {
            if (m_test_renderable != nullptr)
            {
                m_test_renderable = nullptr;
            }
            else
            {
                m_test_renderable = std::make_unique<Renderer::CTriangle2d>();
            }
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("About"))
        {
            m_display_about = true;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Display ImGui demo ", "", &m_display_imgui_demo))
        {
//            m_display_imgui_demo = !m_display_imgui_demo;
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    
    if (m_display_about == true)
    {
        m_display_about = false;
        ImGui::OpenPopup("About - GOAT");
    }

    bool unused_close = true;
    if (ImGui::BeginPopupModal("About - GOAT", &unused_close, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        ImGui::Text("GOAT engine editor\n" \
                    "Version 0.1a\n" \
                    "(c) 2021 Tom Haygarth");

        ImGui::EndPopup();
    }
    
    if (m_display_imgui_demo == true)
    {
        ImGui::ShowDemoWindow();
    }
}

void EditorWindow::Render(Renderer::IRenderContext * render_context)
{
    if (m_test_renderable != nullptr)
    {
        render_context->SubmitRenderable(m_test_renderable.get());
    }
}

bool EditorWindow::WindowShouldClose()
{
    return m_should_close;
}
