#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <memory>

#include "renderer/render_context.hpp"
#include "utility/logging.hpp"

#define UNUSED(expr) (void)expr

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
int main()
{
    std::cout << "Init GLFW begin" << std::endl;

    GLFWwindow * window = nullptr;
    glfwSetErrorCallback(error_callback);

    if (glfwInit() == false)
    {
    std::cout << "Failed to init GLFW" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Init GLFW finished" << std::endl;
    std::cout << "Creating GLFW window" << std::endl;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(800, 600, "OGAT - GOAT", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    std::cout << "Created GLFW window" << std::endl;

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::unique_ptr<Renderer::VulkanRenderContext> renderer = std::make_unique<Renderer::VulkanRenderContext>(window);
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

    glfwSetKeyCallback(window, key_callback);

    while (glfwWindowShouldClose(window) == false && renderer->HasError() == false)
    {
        glfwPollEvents();
        renderer->RenderFrame();
    }

    std::cout << "Quitting" << std::endl;
    glfwDestroyWindow(window);
    window = nullptr;

    glfwTerminate();

    return EXIT_SUCCESS;
}
