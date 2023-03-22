#ifndef CUBE_GRAPHICS_TYPES_H
#define CUBE_GRAPHICS_TYPES_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

typedef struct _cube_frame
{
    VkImageView image_view;
    VkFramebuffer framebuffer;
    VkCommandBuffer command_buffer;
    VkFence fence;
} cube_frame;

typedef struct _cube_ubo
{
    uint32_t model[4][4];
    uint32_t view[4][4];
    uint32_t projection[4][4];
} cube_ubo;

typedef struct _cube_graphics
{
    SDL_Window *window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkExtent2D display_size;

    VkPhysicalDevice physical_device;
    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;
    VkDevice logical_device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkCommandPool command_pool;

    VkRenderPass render_pass;
    VkPipeline graphics_pipeline;

    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSurfaceFormatKHR surface_format;
    VkSwapchainKHR swapchain;
    uint32_t frame_count;
    cube_frame *frames;
} cube_graphics;

#endif