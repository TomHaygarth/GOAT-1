#include "window/window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <memory>
#include <utility>

#include "renderer/render_context.hpp"
#include "utility/logging.hpp"

#define UNUSED(expr) (void)expr

Renderer::VulkanRenderContext * renderer = nullptr;

static void error_callback(int, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // TODO: Remove unused macro when these are used
    UNUSED(scancode);
    UNUSED(mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void resize_callback(GLFWwindow * _window, int _width, int _height)
{
    UNUSED(_window);
    renderer->ResizeScreen(static_cast<uint32_t>(_width),
                           static_cast<uint32_t>(_height));
}


Window::WindowInstance::WindowInstance(std::string const & window_name,
                                       int width,
                                       int height,
                                       std::unique_ptr<IWindowFunctions> && window_funcs)
: m_title(window_name)
, m_width(width)
, m_height(height)
, m_window_funcs(std::move(window_funcs))
{

}

int Window::WindowInstance::Run()
{
    std::cout << "Init GLFW begin" << std::endl;

    glfwSetErrorCallback(error_callback);

    if (glfwInit() == false)
    {
    std::cout << "Failed to init GLFW" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Init GLFW finished" << std::endl;
    std::cout << "Creating GLFW window" << std::endl;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow * glfw_window = nullptr;
    glfw_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (glfw_window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    std::cout << "Created GLFW window" << std::endl;

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    renderer = new Renderer::VulkanRenderContext(glfw_window);
    if (renderer->HasError() == false)
    {
        renderer->Init();
    }

    if (renderer->HasError())
    {
        ERROR_LOG(renderer->GetLastError());
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // glm::mat4 matrix;
    // glm::vec4 vec;
    // auto test = matrix * vec;

    glfwSetKeyCallback(glfw_window, key_callback);
    glfwSetWindowSizeCallback(glfw_window, resize_callback);

    while (glfwWindowShouldClose(glfw_window) == false && renderer->HasError() == false)
    {
        glfwPollEvents();
        if (m_window_funcs != nullptr)
        {
            m_window_funcs->Input();
            m_window_funcs->Render();
        }
        renderer->RenderFrame();
    }

    std::cout << "Quitting" << std::endl;
    glfwDestroyWindow(glfw_window);
    glfw_window = nullptr;

    glfwTerminate();

    return EXIT_SUCCESS;
}

Window::WindowInstance * Window::create_window(std::string const & window_name,
                                               int width,
                                               int height,
                                               std::unique_ptr<IWindowFunctions> && window_funcs)
{
    return new WindowInstance(window_name, width, height, std::move(window_funcs));
}

void Window::destroy_window(Window::WindowInstance * window)
{
    delete window;
    window = nullptr;
}
