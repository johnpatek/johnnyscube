#include "cube.h"

static int graphics_create_surface_properties(cube_graphics *graphics);
static int graphics_create_frame(cube_graphics *graphics, VkImage image, cube_frame *frame);

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
        .minImageCount = graphics->surface_capabilities.minImageCount,
        .imageFormat = graphics->surface_format.format,
        .imageColorSpace = graphics->surface_format.colorSpace,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageExtent = graphics->display_size,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
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

int graphics_create_frame_pool(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    uint32_t swapchain_image_count;
    VkImage *swapchain_images;
    uint32_t frame_index;

    CUBE_ASSERT(
        graphics_create_swapchain(graphics) == CUBE_SUCCESS,
        "failed to create swapchain")

    CUBE_ASSERT(
        vkGetSwapchainImagesKHR(
            graphics->logical_device,
            graphics->swapchain,
            &swapchain_image_count,
            NULL) == VK_SUCCESS,
        "failed to get swapchain image count")

    graphics->frame_count = swapchain_image_count;

    swapchain_images = CUBE_CALLOC(swapchain_image_count, sizeof(VkImage));
    CUBE_ASSERT(swapchain_images != NULL, "failed to allocate swapchain images")

    CUBE_ASSERT(
        vkGetSwapchainImagesKHR(
            graphics->logical_device,
            graphics->swapchain,
            &swapchain_image_count,
            swapchain_images) == VK_SUCCESS,
        "failed to get swapchain images")

    for (frame_index = 0; frame_index < swapchain_image_count; frame_index++)
    {
        CUBE_ASSERT(
            graphics_create_frame(
                graphics,
                *(swapchain_images + frame_index),
                (graphics->frames + frame_index)),
            "failed to create frame")
    }
    CUBE_END_FUNCTION
}

int graphics_render_acquire_frame(cube_graphics *graphics, cube_frame **frame)
{
}

int graphics_render_submit_frame(cube_graphics *graphics, cube_frame *frame)
{
}

void graphics_destroy_swapchain(cube_graphics *graphics)
{
    
}

void graphics_destroy_frame_pool(cube_graphics *graphics)
{

}

static int graphics_create_frame(cube_graphics *graphics, VkImage image, cube_frame *frame)
{
    CUBE_BEGIN_FUNCTION
    const VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = graphics->surface_format.format,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .image = image,
    };
    const VkFramebufferCreateInfo framebuffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = graphics->render_pass,
        .pAttachments = &frame->image_view,
    };

    VK_CHECK_RESULT(
        vkCreateImageView(
            graphics->logical_device,
            &image_view_create_info,
            NULL,
            &frame->image_view))
    CUBE_END_FUNCTION
}