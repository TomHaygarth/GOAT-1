#include "render_context.hpp"

#include "utility/logging.hpp"

#include <cinttypes>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

#include <GLFW/glfw3.h>

namespace
{
    struct SQueueFamilyIndices {
        std::pair<bool, uint32_t> graphicsFamily = {false, 0};
    };

    SQueueFamilyIndices find_queue_families(VkPhysicalDevice device)
    {
        SQueueFamilyIndices indices;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device,
                                                 &queue_family_count,
                                                 queue_families.data());
        uint32_t i = 0;
        for (auto const & queue : queue_families)
        {
            if ((queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = std::make_pair(true, i);
                break;
            }
        }

        return indices;
    }

    uint64_t score_physical_device(VkPhysicalDevice device)
    {
        uint64_t score = 0;

        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        vkGetPhysicalDeviceFeatures(device, &device_features);

        Utility::DEBUG_LOG(std::string(device_properties.deviceName));

        if  (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 10000;
        }

        score += static_cast<uint64_t>(device_properties.limits.maxImageDimension2D);

        SQueueFamilyIndices const family_queue = find_queue_families(device);
        if (family_queue.graphicsFamily.first == false)
        {
            score = 0;
        }

        std::stringstream sstrm;
        sstrm << "Found physical device " << device_properties.deviceName << " with score : " << score;
        Utility::DEBUG_LOG(sstrm.str());

        return score;
    }
}
Renderer::VulkanRenderContext::VulkanRenderContext()
{
    VkApplicationInfo app_info;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "GOAT Engine app";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "GOAT Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    uint32_t glfw_extension_count = 0;
    const char ** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.pNext = nullptr;

    create_info.enabledLayerCount = 0;

    if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
    {
        m_last_error = "Failed to create Vulkan instance";
    }
}

Renderer::VulkanRenderContext::~VulkanRenderContext()
{
    vkDestroyInstance(m_instance, nullptr);
}

bool Renderer::VulkanRenderContext::Init()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
    if (device_count == 0)
    {
        m_last_error = "No physical devices found";
        return false;
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());
    uint64_t device_score = 0;
    for (auto & device : devices)
    {
        uint64_t _score = score_physical_device(device);
        if (_score > device_score)
        {
            m_physical_device = device;
            device_score = _score;
        }
    }

    if (m_physical_device == VK_NULL_HANDLE)
    {
         m_last_error = "All devices found were unsuitable";
        return false;
    }
    else
    {
        Utility::DEBUG_LOG("Selected a physical device");
    }
    return true;
}
