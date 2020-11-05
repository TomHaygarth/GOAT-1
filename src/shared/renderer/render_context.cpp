#include "render_context.hpp"

#include "utility/logging.hpp"
#include "utility/optional.hpp"
#include "utility/file/file_helper.hpp"

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
#pragma mark -- graphics pipeline helpers
#pragma region
// ------------------------------------------------------------------------------------------------------------------------- //

VkShaderModule create_shader_module(VkDevice device, std::vector<char> const & shader_code)
{
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader_code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(shader_code.data());

    VkShaderModule module;
    if (vkCreateShaderModule(device, &create_info, nullptr, &module) != VK_SUCCESS)
    {
        ERROR_LOG("Failed to create shadre module.");
        module = VK_NULL_HANDLE;
    }
    return module;
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
    if (m_render_finished_semaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(m_logical_device, m_render_finished_semaphore, nullptr);
    }

    if (m_image_available_semaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(m_logical_device, m_image_available_semaphore, nullptr);
    }

    if (m_command_pool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(m_logical_device, m_command_pool, nullptr);
    }

    for (auto framebuffer : m_swapchain_framebuffers)
    {
        if (framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(m_logical_device, framebuffer, nullptr);
        }
    }

    if (m_graphics_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_logical_device, m_graphics_pipeline, nullptr);
    }

    if (m_pipeline_layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_logical_device, m_pipeline_layout, nullptr);
    }

    if (m_render_pass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(m_logical_device, m_render_pass, nullptr);
    }

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

    if (CreateRenderPass() == true)
    {
        DEBUG_LOG("Render Pass created");
    }
    else
    {
        return false;
    }

    if (CreateGraphicsPipeline() == true)
    {
        DEBUG_LOG("Graphics Pipeline created");
    }
    else
    {
        return false;
    }

    if (CreateFramebuffers() == true)
    {
        DEBUG_LOG("Framebuffers created");
    }
    else
    {
        return false;
    }

    if (CreateCommandPool() == true)
    {
        DEBUG_LOG("Command pool created");
    }
    else
    {
        return false;
    }

    if (CreateCommandBuffers() == true)
    {
        DEBUG_LOG("Command buffers created");
    }
    else
    {
        return false;
    }

    if (CreateSemaphores() == true)
    {
        DEBUG_LOG("Semaphores created");
    }
    else
    {
        return false;
    }

    return true;
}

void Renderer::VulkanRenderContext::RenderFrame()
{
    vkDeviceWaitIdle(m_logical_device);
    uint32_t image_index = 0;
    vkAcquireNextImageKHR(m_logical_device,
                          m_swapchain,
                          UINT64_MAX,
                          m_image_available_semaphore,
                          VK_NULL_HANDLE,
                          &image_index);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = { m_image_available_semaphore };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_command_buffers[image_index];

    VkSemaphore signal_semaphores[] = { m_render_finished_semaphore };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        m_last_error = "failed to submit draw command buffer!";
        ERROR_LOG(m_last_error);
        return;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapChains[] = { m_swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &image_index;

    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(m_present_queue, &presentInfo);
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

bool Renderer::VulkanRenderContext::CreateRenderPass()
{
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = m_swapchain_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(m_logical_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
    {
        m_last_error = "failed to create render pass";
        ERROR_LOG(m_last_error);
        return false;
    }
    return true;
}

bool Renderer::VulkanRenderContext::CreateGraphicsPipeline()
{
    bool result = true;

    auto vert_shader_code = FileHelpers::read_file("shaders/basic_unlit_vert.spv");
    auto frag_shader_code = FileHelpers::read_file("shaders/basic_unlit_frag.spv");

    result = vert_shader_code.empty() == false && frag_shader_code.empty() == false;

    VkShaderModule vert_shader_module = VK_NULL_HANDLE;
    VkShaderModule frag_shader_module = VK_NULL_HANDLE;

    if (result == true)
    {
        vert_shader_module = create_shader_module(m_logical_device, vert_shader_code);
        frag_shader_module = create_shader_module(m_logical_device, frag_shader_code);
    }

    result = vert_shader_module != VK_NULL_HANDLE && frag_shader_module != VK_NULL_HANDLE;

    if (result == true)
    {
        VkPipelineShaderStageCreateInfo vert_shader_create_info = {};
        vert_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_create_info.module = vert_shader_module;
        vert_shader_create_info.pName = "main";


        VkPipelineShaderStageCreateInfo frag_shader_create_info = {};
        frag_shader_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_create_info.module = frag_shader_module;
        frag_shader_create_info.pName = "main";

        VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_create_info, frag_shader_create_info };

        VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        vertex_input_info.pVertexBindingDescriptions = nullptr;
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        vertex_input_info.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_surface_extent.width);
        viewport.height = static_cast<float>(m_surface_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = m_surface_extent;

        VkPipelineViewportStateCreateInfo viewport_state = {};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState colour_blend_attachment{};
        colour_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colour_blend_attachment.blendEnable = VK_TRUE;
        colour_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colour_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colour_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        colour_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colour_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colour_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colour_blending{};
        colour_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colour_blending.logicOpEnable = VK_FALSE;
        colour_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colour_blending.attachmentCount = 1;
        colour_blending.pAttachments = &colour_blend_attachment;
        colour_blending.blendConstants[0] = 0.0f; // Optional
        colour_blending.blendConstants[1] = 0.0f; // Optional
        colour_blending.blendConstants[2] = 0.0f; // Optional
        colour_blending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0; // Optional
        pipeline_layout_info.pSetLayouts = nullptr; // Optional
        pipeline_layout_info.pushConstantRangeCount = 0; // Optional
        pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(m_logical_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        {
            m_last_error = "failed to create pipeline layout!";
            ERROR_LOG(m_last_error);
            result = false;
        }

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;

        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = nullptr; // Optional
        pipeline_info.pColorBlendState = &colour_blending;
        pipeline_info.pDynamicState = nullptr; // Optional

        pipeline_info.layout = m_pipeline_layout;

        pipeline_info.renderPass = m_render_pass;
        pipeline_info.subpass = 0;

        pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipeline_info.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(m_logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) != VK_SUCCESS)
        {
            m_last_error = "failed to create graphics pipeline!";
            ERROR_LOG(m_last_error);
            result = false;
        }
    }
    else
    {
        m_last_error = "Failed to load shader files";
        ERROR_LOG(m_last_error);
    }

    if(frag_shader_module != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(m_logical_device, frag_shader_module, nullptr);
    }

    if (vert_shader_module != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(m_logical_device, vert_shader_module, nullptr);
    }
    return result;
}

bool Renderer::VulkanRenderContext::CreateFramebuffers()
{
    bool success = true;
    m_swapchain_framebuffers.resize(m_swapchain_image_views.size(), VK_NULL_HANDLE);

    for (size_t i = 0; i < m_swapchain_image_views.size() && success == true; ++i)
    {
        VkImageView attachments[] = { m_swapchain_image_views[i] };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = m_render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = m_surface_extent.width;
        framebuffer_info.height = m_surface_extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(m_logical_device, &framebuffer_info, nullptr, &m_swapchain_framebuffers[i]) != VK_SUCCESS)
        {
            m_last_error = "Failed to create framebuffer";
            ERROR_LOG(m_last_error);
            success = false;
        }
    }
    return success;
}

bool Renderer::VulkanRenderContext::CreateCommandPool()
{
    bool success = true;

    SQueueFamilyIndices queue_family_indices = find_queue_families(m_physical_device, m_surface);

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_indices.graphicsFamily.Value();
    pool_info.flags = 0;

    if (vkCreateCommandPool(m_logical_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
    {
        m_last_error = "Failed to create command pool";
        ERROR_LOG(m_last_error);
        success = false;
    }
    return success;
}

bool Renderer::VulkanRenderContext::CreateCommandBuffers()
{
    bool success = true;
    m_command_buffers.resize(m_swapchain_framebuffers.size(), VK_NULL_HANDLE);

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = m_command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

    if (vkAllocateCommandBuffers(m_logical_device, &alloc_info, m_command_buffers.data()) != VK_SUCCESS)
    {
        m_last_error = "failed to allocate command buffers!";
        ERROR_LOG(m_last_error);
        success = false;
    }

    for (size_t i = 0; i < m_command_buffers.size() && success == true; i++)
    {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0; // Optional
        begin_info.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(m_command_buffers[i], &begin_info) != VK_SUCCESS)
        {
            m_last_error = "Failed to begin recording command buffer";
            ERROR_LOG(m_last_error);
            success = false;
            break;
        }

        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = m_render_pass;
        render_pass_info.framebuffer = m_swapchain_framebuffers[i];

        render_pass_info.renderArea.offset = { 0, 0 };
        render_pass_info.renderArea.extent = m_surface_extent;

        VkClearValue clear_colour = { 0.0f, 0.0f, 0.0f, 1.0f };
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clear_colour;

        vkCmdBeginRenderPass(m_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
        vkCmdDraw(m_command_buffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(m_command_buffers[i]);

        if (vkEndCommandBuffer(m_command_buffers[i]) != VK_SUCCESS)
        {
            m_last_error = "failed to record command buffer!";
            ERROR_LOG(m_last_error);
            success = false;
        }
    }
    return success;
}

bool Renderer::VulkanRenderContext::CreateSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(m_logical_device, &semaphoreInfo, nullptr, &m_image_available_semaphore) != VK_SUCCESS
    ||  vkCreateSemaphore(m_logical_device, &semaphoreInfo, nullptr, &m_render_finished_semaphore) != VK_SUCCESS)
    {
        m_last_error = "failed to create semaphores!";
        ERROR_LOG(m_last_error);
        return false;
    }

    return true;
}
