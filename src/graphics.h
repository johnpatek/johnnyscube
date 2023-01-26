#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"

struct graphics_s
{
    SDL_DisplayMode display_mode;
    SDL_Window *window;

    VkInstance vk_instance;
    VkSurfaceKHR vk_surface;

    VkPhysicalDevice vk_physical_device;
    int vk_graphics_queue_index;
    int vk_present_queue_index;
    VkDevice vk_device;
    VkQueue vk_graphics_queue;
    VkQueue vk_present_queue;

    VkSurfaceFormatKHR vk_surface_format;
    VkSwapchainKHR vk_swapchain;
    VkExtent2D vk_swapchain_size;

    uint32_t vk_image_count;
    VkImage *vk_images;
    VkImageView *vk_image_views;

    VkFormat vk_depth_format;
    VkImage vk_depth_image;
    VkDeviceMemory vk_depth_image_memory;
    VkImageView vk_depth_image_view;

    VkRenderPass vk_render_pass;
    VkPipeline vk_graphics_pipeline;
    VkFramebuffer *vk_framebuffers;

    VkCommandPool vk_command_pool;

    VkCommandBuffer *vk_command_buffers;

    VkSemaphore vk_image_semaphore;
    VkSemaphore vk_render_semaphore;

    VkFence *vk_fences;
    
    uint32_t vk_current_index;
};
typedef struct graphics_s *graphics_t;

int graphics_create(graphics_t *graphics, const char * const resource_directory);

int graphics_render(graphics_t graphics);

void graphics_destroy(graphics_t graphics);

#endif