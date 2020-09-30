#include "render_context.hpp"

#include "utility/logging.hpp"
#include "utility/optional.hpp"

#include <array>
#include <cinttypes>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <utility>

#include <GLFW/glfw3.h>

constexpr bool bVerbose = false;

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
        std::set<uint32_t> queue_families = {
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
// ------------------------------------------------------------------------------------------------------------------------- //
#pragma mark -- device extension helpers
#pragma region
// ------------------------------------------------------------------------------------------------------------------------- //
    constexpr std::array<const char*, 1> required_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool const check_device_extension_support(VkPhysicalDevice device)
    {
        std::set<std::string> required_extensions(required_device_extensions.begin(),
            required_device_extensions.end());

        uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        for (auto const & extension : available_extensions)
        {
            if (bVerbose == true)
            {
                DEBUG_LOG("Found extension: " + std::string(extension.extensionName));
            }
            required_extensions.erase(extension.extensionName);
        }

        for (auto const & extension_name : required_extensions)
        {
            ERROR_LOG("Missing required: " + extension_name);
        }

        return required_extensions.empty();
    }
#pragma endregion

 // ------------------------------------------------------------------------------------------------------------------------- //
#pragma mark -- swapchain helpers
#pragma region
// ------------------------------------------------------------------------------------------------------------------------- //

    struct SSwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    SSwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SSwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

        if (format_count != 0)
        {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
        }

        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

        if (present_mode_count != 0)
        {
            details.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
        }
        return details;
    }

    VkSurfaceFormatKHR choose_swap_surface_format(std::vector<VkSurfaceFormatKHR> const & available_formats)
    {
        assert(available_formats.empty() == false);

        for (auto const & available_format : available_formats)
        {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB
                && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return available_format;
            }
        }
        return available_formats[0];
    }

    VkPresentModeKHR choose_swap_present_mode(std::vector<VkPresentModeKHR> const & available_modes)
    {
        assert(available_modes.empty() == false);

        for (auto const & available_mode : available_modes)
        {
            // check if we support tripple buffering
            if (available_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return available_mode;
            }
        }

        // queue frames like for vblank/vsync (always guaranteed to ast least support it
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR const & capabillities, uint32_t const width, uint32_t const height)
    {
        if (capabillities.currentExtent.width != UINT32_MAX)
        {
            return capabillities.currentExtent;
        }
        else
        {
            VkExtent2D actual_extent;

            actual_extent.width = std::max(capabillities.minImageExtent.width,
                                           std::min(capabillities.maxImageExtent.width, width));

            actual_extent.height = std::max(capabillities.minImageExtent.height,
                                            std::min(capabillities.maxImageExtent.width, height));

            return actual_extent;
        }
    }
#pragma endregion

// ------------------------------------------------------------------------------------------------------------------------- //
#pragma mark -- device suitability helpers
#pragma region
// ------------------------------------------------------------------------------------------------------------------------- //
    bool const is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SQueueFamilyIndices const family_queue = find_queue_families(device, surface);
        bool const extensions_supported = check_device_extension_support(device);

        bool swap_chain_adequate = false;
        if (extensions_supported == true)
        {
            SSwapChainSupportDetails swap_chain_support = query_swapchain_support(device, surface);
            swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
        }

        return extensions_supported == true
            && swap_chain_adequate == true
            && family_queue.graphicsFamily.HasValue() == true
            && family_queue.presentFamily.HasValue() == true;
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

        if (is_device_suitable(device, surface) == false)
        {
            score = 0;
        }

        std::stringstream sstrm;
        sstrm << "Found physical device " << device_properties.deviceName << " with score : " << score;
        DEBUG_LOG(sstrm.str());

        return score;
    }
}
#pragma endregion

// ------------------------------------------------------------------------------------------------------------------------- //

Renderer::VulkanRenderContext::VulkanRenderContext(GLFWwindow * glfw_window)
: m_ptr_glfw_window(glfw_window)
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
    for (auto image_view : m_swapchain_image_views)
    {
        if (image_view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_logical_device, image_view, nullptr);
        }
    }
    if (m_swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_logical_device, m_swapchain, nullptr);
    }
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

    if (CreateSwapChain() == true)
    {
        DEBUG_LOG("Swap chain created");
    }
    else
    {
        return false;
    }

    if (CreateImageViews() == true)
    {
        DEBUG_LOG("Image Views created");
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

    device_create_info.enabledExtensionCount = static_cast<uint32_t>(required_device_extensions.size());
    device_create_info.ppEnabledExtensionNames = required_device_extensions.data();

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

bool Renderer::VulkanRenderContext::CreateSwapChain()
{
    SSwapChainSupportDetails swapchain_support = query_swapchain_support(m_physical_device, m_surface);

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swapchain_support.formats);
    VkPresentModeKHR surface_present = choose_swap_present_mode(swapchain_support.present_modes);
    m_swapchain_format = surface_format.format;

    int width = 0;
    int height = 0;
    glfwGetWindowSize(m_ptr_glfw_window, &width, &height);

    m_surface_extent = choose_swap_extent(swapchain_support.capabilities,
                                          static_cast<uint32_t>(width),
                                          static_cast<uint32_t>(height));

    // get at least min+1 images for the swapchain unless we exceed the max images allowed, in that case default to max
    uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
    if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount)
    {
        image_count = swapchain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = m_surface_extent;
    // TODO: In future allow this to be set via some config in case we're developing a vr app we might want more than 1 layer
    create_info.imageArrayLayers = 1;
    // For now just set the image usage for rendering directly to the screen. In future we could allow different
    // usages like rendering post-processing effect and then copy the result to another image
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    SQueueFamilyIndices indices = find_queue_families(m_physical_device, m_surface);
    uint32_t queue_family_indeces[] = { indices.graphicsFamily.Value(), indices.presentFamily.Value() };

    // if the swapchain is used across 2 different queues for now we'll just use the concurrent sharing mode
    if (indices.graphicsFamily.Value() != indices.presentFamily.Value())
    {
        DEBUG_LOG("Setting swapchain sharing mode to concurrent");
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indeces;
    }
    else
    {
        DEBUG_LOG("Setting swapchain sharing mode to exclusive");
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = swapchain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = surface_present;
    create_info.clipped = VK_TRUE;

    // Only used if we need to rebuild the swapchain if something like a window resize happens
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_logical_device, &create_info, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        m_last_error = "Failed to create swap chain";
        ERROR_LOG(m_last_error);
        return false;
    }

    vkGetSwapchainImagesKHR(m_logical_device, m_swapchain, &image_count, nullptr);
    m_swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(m_logical_device, m_swapchain, &image_count, m_swapchain_images.data());

    return true;
}

bool Renderer::VulkanRenderContext::CreateImageViews()
{
    m_swapchain_image_views.resize(m_swapchain_images.size(), VK_NULL_HANDLE);

    for (size_t i = 0; i < m_swapchain_image_views.size(); ++i)
    {
        VkImageViewCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = m_swapchain_images[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = m_swapchain_format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_logical_device, &create_info, nullptr, &m_swapchain_image_views[i]) != VK_SUCCESS)
        {
            m_last_error = "Failed to create image views";
            ERROR_LOG(m_last_error);
            return false;
        }
    }
    return true;
}