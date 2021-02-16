#include "window/window.hpp"

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <memory>
#include <utility>

#include "utility/logging.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"

#ifdef GOAT_USE_OPENGL
#include "backends/imgui_impl_opengl3.h"
#include "renderer/opengl/opengl_render_context.hpp"

Renderer::OpenGLRenderContext * renderer = nullptr;

#endif

#ifdef GOAT_USE_VULKAN
#include "backends/imgui_impl_vulkan.h"
#include "renderer/vulkan/vulkan_render_context.hpp"

Renderer::VulkanRenderContext * renderer = nullptr;

#endif

#define UNUSED(expr) (void)expr

Window::IWindowFunctions::~IWindowFunctions()
{
}

void init_imgui(GLFWwindow * window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
#ifdef GOAT_USE_OPENGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(renderer->GetGLSLVersion().c_str());
#endif
#ifdef GOAT_USE_VULKAN
    // TODO: expose the vulkan physical and logical devices along with all necesarry queues etc 
    // ImGui_ImplGlfw_InitForVulkan(window, true);
    // ImGui_ImplVulkan_InitInfo init_info = {};
    // init_info.Instance = g_Instance;
    // init_info.PhysicalDevice = g_PhysicalDevice;
    // init_info.Device = g_Device;
    // init_info.QueueFamily = g_QueueFamily;
    // init_info.Queue = g_Queue;
    // init_info.PipelineCache = g_PipelineCache;
    // init_info.DescriptorPool = g_DescriptorPool;
    // init_info.Allocator = g_Allocator;
    // init_info.MinImageCount = g_MinImageCount;
    // init_info.ImageCount = wd->ImageCount;
    // init_info.CheckVkResultFn = check_vk_result;
    // ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
#endif
}

void imgui_begin_frame()
{
#ifdef GOAT_USE_OPENGL
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
#endif
#ifdef GOAT_USE_VULKAN
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
#endif

    ImGui::NewFrame();
}

void imgui_end_frame()
{
    ImGui::Render();
}

void imgui_draw_frame()
{
#ifdef GOAT_USE_OPENGL
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
#ifdef GOAT_USE_VULKAN
    // TODO: implement when vulkan renderer is ready
    // ImDrawData* draw_data = ImGui::GetDrawData();
    // renderer->RenderFrame(draw_data);
#endif
}

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
                                       IWindowFunctions * window_funcs)
: m_title(window_name)
, m_width(width)
, m_height(height)
, m_window_funcs(window_funcs)
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

#ifdef GOAT_USE_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif

    GLFWwindow * glfw_window = nullptr;
    glfw_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (glfw_window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    std::cout << "Created GLFW window" << std::endl;

    
#ifdef GOAT_USE_VULKAN
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << "Instantiating Vulkan Renderer" << std::endl;
    renderer = new Renderer::VulkanRenderContext(glfw_window);
#else
    std::cout << "Instantiating OpenGL Renderer" << std::endl;
    renderer = new Renderer::OpenGLRenderContext(glfw_window);
#endif

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

    init_imgui(glfw_window);

    glfwSetKeyCallback(glfw_window, key_callback);
    glfwSetWindowSizeCallback(glfw_window, resize_callback);

    while (glfwWindowShouldClose(glfw_window) == false && renderer->HasError() == false)
    {
        glfwPollEvents();

        imgui_begin_frame();

        if (m_window_funcs != nullptr)
        {
            m_window_funcs->Input();
        }

        imgui_end_frame();

        renderer->PreRender();

        if (m_window_funcs != nullptr)
        {
            m_window_funcs->Render();
        }

        imgui_draw_frame();
        renderer->RenderFrame();

        if (m_window_funcs != nullptr)
        {
            if (m_window_funcs->WindowShouldClose() == true)
            {
                glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
            }
        }
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
                                               IWindowFunctions * window_funcs)
{
    return new WindowInstance(window_name, width, height, window_funcs);
}

void Window::destroy_window(Window::WindowInstance * window)
{
    delete window;
    window = nullptr;
}
