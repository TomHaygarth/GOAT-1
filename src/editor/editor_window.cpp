#include "editor_window.hpp"

#include "imgui.h"

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

void EditorWindow::Render()
{
}

bool EditorWindow::WindowShouldClose()
{
    return m_should_close;
}
