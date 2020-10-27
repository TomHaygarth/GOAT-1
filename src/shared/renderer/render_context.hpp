#pragma once

#include <vulkan/vulkan.h>
#include<string>
#include<vector>

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
        bool CreateSwapChain();
        bool CreateImageViews();
        bool CreateGraphicsPipeline();

        GLFWwindow * m_ptr_glfw_window = nullptr;

        VkInstance m_instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice m_logical_device = VK_NULL_HANDLE;
        VkQueue m_graphics_queue = VK_NULL_HANDLE;
        VkQueue m_present_queue = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkFormat m_swapchain_format;
        VkExtent2D m_surface_extent;

        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        std::string m_last_error;
    };
}
