#include "opengl_render_context.hpp"

#include "utility/logging.hpp"
#include "utility/optional.hpp"
#include "utility/file/file_helper.hpp"

#include <array>
#include <cinttypes>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <utility>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Renderer::OpenGLRenderContext::OpenGLRenderContext(GLFWwindow * glfw_window)
: m_screen_width(0)
, m_screen_height(0)
, m_glsl_version("#version 150")
, m_ptr_glfw_window(glfw_window)
{
}

bool Renderer::OpenGLRenderContext::Init()
{
    glfwMakeContextCurrent(m_ptr_glfw_window);
    glfwSwapInterval(1); // Enable vsync

    if (gladLoadGL() == 0)
    {
        ERROR_LOG("Failed to initialise GLAD");

        return false;
    }
    return true;
}

void Renderer::OpenGLRenderContext::ResizeScreen(uint32_t const width, uint32_t const height)
{
    m_screen_width = width;
    m_screen_height = height;
}

void Renderer::OpenGLRenderContext::PreRender()
{
    int display_w, display_h;
    glfwGetFramebufferSize(m_ptr_glfw_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.20f, 0.15f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::OpenGLRenderContext::RenderFrame()
{
    glfwSwapBuffers(m_ptr_glfw_window);
}
