#pragma once

#include <vulkan/vulkan.h>
#include<string>

struct GLFWwindow;

namespace Renderer
{
    class VulkanRenderContext
    {
    public:
        VulkanRenderContext(GLFWwindow * glfw_window);
        ~VulkanRenderContext();

        bool Init();
        bool HasError() const { return m_last_error.empty() == false; }
        std::string const GetLastError() const { return m_last_error; }

    private:

        bool CreateLogicalDevice();

        VkInstance m_instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice m_logical_device = VK_NULL_HANDLE;
        VkQueue m_graphics_queue = VK_NULL_HANDLE;
        VkQueue m_present_queue = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;

        std::string m_last_error;
    };
}
