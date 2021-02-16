#pragma once

#include<string>
#include<vector>

struct GLFWwindow;

namespace Renderer
{
    class OpenGLRenderContext
    {
    public:
        OpenGLRenderContext(GLFWwindow * glfw_window);
        ~OpenGLRenderContext();

        bool Init();
        void ResizeScreen(uint32_t const width, uint32_t const height);
        void PreRender();
        void RenderFrame();
        bool HasError() const { return m_last_error.empty() == false; }
        std::string const GetLastError() const { return m_last_error; }

        std::string const GetGLSLVersion() const { return m_glsl_version; }

    private:
        std::string m_last_error;
        uint32_t m_screen_width;
        uint32_t m_screen_height;

        std::string m_glsl_version;

        GLFWwindow * m_ptr_glfw_window;
    };
}
