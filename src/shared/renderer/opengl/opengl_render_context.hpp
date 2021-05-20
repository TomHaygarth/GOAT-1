#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "renderer/render_context.hpp"

struct GLFWwindow;

namespace Renderer
{
    struct SRenderData
    {
        // TODO: in future shaders will be held separate from the render data
        // as we will share shaders across multiple render datas
        uint32_t temp_shader_id = 0;

        uint32_t vao = 0;
        uint32_t vbo = 0;
        std::vector<glm::vec3> verts;
        std::vector<uint32_t> indicies;
        std::vector<glm::vec4> cols;
    };

    class OpenGLRenderContext : public IRenderContext
    {
    public:
        OpenGLRenderContext(GLFWwindow * glfw_window);
        ~OpenGLRenderContext();

        virtual bool Init() override;
        virtual void ResizeScreen(uint32_t const width, uint32_t const height) override;
        virtual void PreRender() override;
        virtual void RenderFrame() override;
        virtual void SubmitRenderable(IRenderable const * renderable) override;

        bool HasError() const { return m_last_error.empty() == false; }
        std::string const GetLastError() const { return m_last_error; }

        std::string const GetGLSLVersion() const { return m_glsl_version; }

    private:
        std::string m_last_error;
        uint32_t m_screen_width;
        uint32_t m_screen_height;

        std::string m_glsl_version;

        GLFWwindow * m_ptr_glfw_window;

        std::unique_ptr<SRenderData> temp_render_data = nullptr;

        std::unique_ptr<SRenderData> init_new_renderdata();
    };
}
