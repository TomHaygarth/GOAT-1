#pragma once

#include <vulkan/vulkan.h>
#include<string>
#include<vector>

#include "renderer/render_context.hpp"

struct GLFWwindow;

namespace Renderer
{
    class VulkanRenderContext : public IRenderContext
    {
    public:
        VulkanRenderContext(GLFWwindow * glfw_window);
        ~VulkanRenderContext();

        virtual bool Init() override;
        virtual void ResizeScreen(uint32_t const width, uint32_t const height) override;
        virtual void PreRender() override;
        virtual void RenderFrame() override;
        virtual void SubmitRenderable(IRenderable const * renderable) override;
        bool HasError() const { return m_last_error.empty() == false; }
        std::string const GetLastError() const { return m_last_error; }

    private:

        void CleanupSwapChain();
        bool RecreateSwapChain();

        bool CreateLogicalDevice();
        bool CreateSwapChain();
        bool CreateImageViews();
        bool CreateRenderPass();
        bool CreateGraphicsPipeline();
        bool CreateFramebuffers();
        bool CreateCommandPool();
        bool CreateCommandBuffers();
        bool CreateSemaphores();

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
        VkRenderPass m_render_pass = VK_NULL_HANDLE;
        VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
        VkPipeline m_graphics_pipeline = VK_NULL_HANDLE;
        VkCommandPool m_command_pool = VK_NULL_HANDLE;
        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence> m_inflight_fences;
        std::vector<VkFence> m_images_inflight;
        size_t m_current_frame = 0;

        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;
        std::vector<VkFramebuffer> m_swapchain_framebuffers;
        std::vector<VkCommandBuffer> m_command_buffers;

        std::string m_last_error;
    };
}
