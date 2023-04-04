#include "cube.h"

static int graphics_create_surface_properties(cube_graphics *graphics);
static int graphics_create_swapchain(cube_graphics *graphics);
static int graphics_create_depth_format(cube_graphics *graphics);
static int graphics_create_depth_image(cube_graphics *graphics);
static int graphics_create_depth_image_view(cube_graphics *graphics);

int graphics_create_images(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(
        graphics_create_surface_properties(graphics) == CUBE_SUCCESS,
        "failed to create surface properties")
    CUBE_ASSERT(
        graphics_create_swapchain(graphics) == CUBE_SUCCESS,
        "failed to create swapchain")
    CUBE_ASSERT(
        graphics_create_depth_format(graphics) == CUBE_SUCCESS,
        "failed to create depth format")
    CUBE_ASSERT(
        graphics_create_depth_image(graphics) == CUBE_SUCCESS,
        "failed to create depth image")
    CUBE_ASSERT(
        graphics_create_depth_image_view(graphics) == CUBE_SUCCESS,
        "failed to create depth image view")
    CUBE_END_FUNCTION
}

void graphics_destroy_images(cube_graphics *graphics)
{
    if (graphics->depth_image_view != VK_NULL_HANDLE)
    {
        vkDestroyImageView(graphics->logical_device, graphics->depth_image_view, NULL);
    }
    if (graphics->depth_image != VK_NULL_HANDLE)
    {
        vmaDestroyImage(graphics->allocator, graphics->depth_image, graphics->depth_image_allocation);
    }
    if (graphics->swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(graphics->logical_device, graphics->swapchain, NULL);
    }
}

int graphics_create_surface_properties(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    uint32_t surface_format_count;
    VkSurfaceFormatKHR *surface_formats;
    uint32_t surface_format_index;
    VkBool32 found_format;

    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            graphics->physical_device,
            graphics->surface,
            &graphics->surface_capabilities))

    graphics->display_size.width = CLAMP(
        graphics->display_size.width,
        graphics->surface_capabilities.minImageExtent.width,
        graphics->surface_capabilities.maxImageExtent.width);

    graphics->display_size.height = CLAMP(
        graphics->display_size.height,
        graphics->surface_capabilities.minImageExtent.height,
        graphics->surface_capabilities.maxImageExtent.height);

    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            graphics->physical_device,
            graphics->surface,
            &surface_format_count,
            NULL))

    surface_formats = CUBE_CALLOC(surface_format_count, sizeof(VkSurfaceFormatKHR));

    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            graphics->physical_device,
            graphics->surface,
            &surface_format_count,
            surface_formats))

    found_format = VK_FALSE;
    for (surface_format_index = 0; surface_format_index < surface_format_count; surface_format_index++)
    {
        if (found_format == VK_FALSE)
        {
            graphics->surface_format = *(surface_formats + surface_format_index);
            if (graphics->surface_format.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                found_format = VK_TRUE;
            }
        }
    }
    CUBE_ASSERT(found_format == VK_TRUE, "failed to find surface format")
    CUBE_END_FUNCTION
}

int graphics_create_swapchain(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const uint32_t queue_family_indices[] = {
        graphics->graphics_queue_family_index,
        graphics->present_queue_family_index,
    };
    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = graphics->surface,
        .minImageCount = CLAMP(
            2,
            graphics->surface_capabilities.minImageCount,
            graphics->surface_capabilities.maxImageCount),
        .imageFormat = graphics->surface_format.format,
        .imageColorSpace = graphics->surface_format.colorSpace,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .imageExtent = {
            .width = graphics->display_size.width,
            .height = graphics->display_size.height,
        },
        .preTransform = graphics->surface_capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_TRUE,
    };

    if (graphics->graphics_queue_family_index != graphics->present_queue_family_index)
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = sizeof(queue_family_indices) / sizeof(queue_family_indices[0]);
        swapchain_create_info.pQueueFamilyIndices = &queue_family_indices[0];
    }

    VK_CHECK_RESULT(
        vkCreateSwapchainKHR(
            graphics->logical_device,
            &swapchain_create_info,
            NULL,
            &graphics->swapchain))
    CUBE_END_FUNCTION
}

int graphics_create_depth_format(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };
    const uint32_t format_count = sizeof(formats) / sizeof(formats[0]);
    uint32_t format_index;
    VkFormatProperties format_properties;

    graphics->depth_format = VK_FORMAT_UNDEFINED;
    graphics->depth_stencil_support = VK_FALSE;
    for (format_index = 0; format_index < format_count; format_index++)
    {
        if (graphics->depth_format == VK_FORMAT_UNDEFINED)
        {
            vkGetPhysicalDeviceFormatProperties(
                graphics->physical_device,
                formats[format_index],
                &format_properties);
            if ((format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                graphics->depth_format = formats[format_index];
            }
        }
    }

    CUBE_ASSERT(
        graphics->depth_format != VK_FORMAT_UNDEFINED,
        "failed to find supported depth format")

    if (graphics->depth_format == VK_FORMAT_D32_SFLOAT_S8_UINT || graphics->depth_format == VK_FORMAT_D24_UNORM_S8_UINT)
    {
        graphics->depth_stencil_support = VK_TRUE;
    }

    CUBE_END_FUNCTION
}

int graphics_create_depth_image(cube_graphics *graphics)
{

    CUBE_BEGIN_FUNCTION
    const VkImageCreateInfo depth_image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = graphics->depth_format,
        .extent = {
            .width = graphics->display_size.width,
            .height = graphics->display_size.height,
            .depth = 1,
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    const VmaAllocationCreateInfo depth_image_allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
    };
    VK_CHECK_RESULT(
        vmaCreateImage(
            graphics->allocator,
            &depth_image_create_info,
            &depth_image_allocation_create_info,
            &graphics->depth_image,
            &graphics->depth_image_allocation,
            NULL))
    CUBE_END_FUNCTION
}

int graphics_create_depth_image_view(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const VkImageViewCreateInfo depth_image_view_creat_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = graphics->depth_image,
        .format = graphics->depth_format,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .baseArrayLayer = 0,
            .layerCount = 1,
            .baseMipLevel = 0,
            .levelCount = 1,
        },
    };
    VK_CHECK_RESULT(
        vkCreateImageView(
            graphics->logical_device,
            &depth_image_view_creat_info,
            NULL,
            &graphics->depth_image_view))
    CUBE_END_FUNCTION
}