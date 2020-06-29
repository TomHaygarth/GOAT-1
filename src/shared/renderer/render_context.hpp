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
        VkInstance m_instance;
        VkPhysicalDevice m_physical_device;
        std::string m_last_error;
    };
}
