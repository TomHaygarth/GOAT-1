#include "render_context.hpp"

#include "utility/logging.hpp"
#include "utility/optional.hpp"

#include <cinttypes>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

#include <GLFW/glfw3.h>

namespace
{
    struct SQueueFamilyIndices {
        GOAT::optional<uint32_t> graphicsFamily;
        GOAT::optional<uint32_t> presentFamily;

        bool isComplete() const
        {
            return graphicsFamily.HasValue()
                && presentFamily.HasValue();
        }
    };

    std::vector<VkDeviceQueueCreateInfo> create_device_queue_info_from_indices(SQueueFamilyIndices const & indices)
    {
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::vector<uint32_t> queue_families = {
            indices.graphicsFamily.Value(),
            indices.presentFamily.Value()
        };

        for (uint32_t const queue_family : queue_families)
        {
            VkDeviceQueueCreateInfo queue_info{};
            queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info.queueFamilyIndex = queue_family;
            queue_info.pNext = nullptr;
            queue_info.queueCount = 1;

            float priorities = 1.0f;
            queue_info.pQueuePriorities = &priorities;

            queue_create_infos.emplace_back(queue_info);
        }
        return queue_create_infos;
    }

    SQueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
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
                indices.graphicsFamily = i;
            }

            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
            if (present_support == true)
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete() == true)
            {
                break;
            }
            i++;
        }

        return indices;
    }

    uint64_t score_physical_device(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint64_t score = 0;

        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        vkGetPhysicalDeviceFeatures(device, &device_features);

        DEBUG_LOG(std::string(device_properties.deviceName));

        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 10000;
        }

        score += static_cast<uint64_t>(device_properties.limits.maxImageDimension2D);

        SQueueFamilyIndices const family_queue = find_queue_families(device, surface);
        if (family_queue.graphicsFamily.HasValue() == false || family_queue.presentFamily.HasValue() == false)
        {
            score = 0;
        }

        std::stringstream sstrm;
        sstrm << "Found physical device " << device_properties.deviceName << " with score : " << score;
        DEBUG_LOG(sstrm.str());

        return score;
    }
}

Renderer::VulkanRenderContext::VulkanRenderContext(GLFWwindow * glfw_window)
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

    if (HasError() == false)
    {
        if (glfwCreateWindowSurface(m_instance, glfw_window, nullptr, &m_surface) != VK_SUCCESS)
        {
            m_last_error = "Failed to create Window Surface";
        }
    }
}

Renderer::VulkanRenderContext::~VulkanRenderContext()
{
    if (m_logical_device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_logical_device, nullptr);
    }

    if (m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }

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
        uint64_t _score = score_physical_device(device, m_surface);
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
        DEBUG_LOG("Selected a physical device");
    }

    if (CreateLogicalDevice() == true)
    {
        DEBUG_LOG("Logical device created");
    }
    else
    {
        return false;
    }

    return true;
}

bool Renderer::VulkanRenderContext::CreateLogicalDevice()
{
    SQueueFamilyIndices const indices = find_queue_families(m_physical_device, m_surface);
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos = create_device_queue_info_from_indices(indices);

    VkPhysicalDeviceFeatures device_features{};
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());

    device_create_info.pEnabledFeatures = &device_features;

    if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_logical_device) != VK_SUCCESS)
    {
        m_last_error = "Failed to create logical device";
        ERROR_LOG(m_last_error);
        return false;
    }
    else
    {
        vkGetDeviceQueue(m_logical_device, indices.graphicsFamily.Value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_logical_device, indices.presentFamily.Value(), 0, &m_present_queue);
    }

    return true;
}
