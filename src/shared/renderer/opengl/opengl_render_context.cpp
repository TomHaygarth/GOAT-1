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

Renderer::OpenGLRenderContext::~OpenGLRenderContext()
{
    if (temp_render_data != nullptr)
    {
        glDeleteProgram(temp_render_data->temp_shader_id);
        glDeleteBuffers(1, &temp_render_data->vbo);
        glDeleteVertexArrays(1, &temp_render_data->vao);
    }
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
    if (temp_render_data != nullptr)
    {
        if (temp_render_data->verts.empty() == false)
        {
            glBindVertexArray(temp_render_data->vao);
            glBindBuffer(GL_ARRAY_BUFFER, temp_render_data->vbo);
            auto & verts = temp_render_data->verts;
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_DYNAMIC_DRAW);
            glUseProgram(temp_render_data->temp_shader_id);
            glDrawArrays(GL_TRIANGLES, 0, verts.size());
        }
        temp_render_data->verts.clear();
        temp_render_data->indicies.clear();
        temp_render_data->cols.clear();
    }

    glfwSwapBuffers(m_ptr_glfw_window);
}

void Renderer::OpenGLRenderContext::SubmitRenderable(IRenderable const * renderable)
{
    if (temp_render_data == nullptr)
    {
        temp_render_data = init_new_renderdata();
    }

    for(auto const & vert : renderable->GetVerts())
    {
        temp_render_data->verts.emplace_back(vert);
    }
    for(auto const & index : renderable->GetIndices())
    {
        temp_render_data->indicies.emplace_back(index);
    }
    for(auto const & col : renderable->GetVertColours())
    {
        temp_render_data->cols.emplace_back(col);
    }
}

std::unique_ptr<Renderer::SRenderData>  Renderer::OpenGLRenderContext::init_new_renderdata()
{
    std::unique_ptr<SRenderData> data = std::make_unique<SRenderData>();

    glGenVertexArrays(1, &data->vao);
    glBindVertexArray(data->vao);
  
    glGenBuffers(1, &data->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, data->vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    uint32_t vertexShader = 0;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::string error_str = "Failed to compile Vertex shader:\n";
        error_str += infoLog;
        ERROR_LOG(error_str);
    }
    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\0";

    uint32_t fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::string error_str = "Failed to compile Fragment shader:\n";
        error_str += infoLog;
        ERROR_LOG(error_str);
    }

    data->temp_shader_id = glCreateProgram();

    glAttachShader(data->temp_shader_id, vertexShader);
    glAttachShader(data->temp_shader_id, fragmentShader);
    glLinkProgram(data->temp_shader_id);

    glGetProgramiv(data->temp_shader_id, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(data->temp_shader_id, 512, NULL, infoLog);
        std::string error_str = "Failed to LINK shaders:\n";
        error_str += infoLog;
        ERROR_LOG(error_str);
    }

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    return data;
}
