#pragma once

#include <vulkan/vulkan.h>
#include<string>

namespace Renderer
{
    class VulkanRenderContext
    {
    public:
        VulkanRenderContext();
        ~VulkanRenderContext();

        bool Init();
        bool HasError() const { return m_last_error.empty() == false; }
        std::string const GetLastError() const { return m_last_error; }

    private:
        VkInstance m_instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        std::string m_last_error;
    };
}
