#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"

typedef struct _vertex_t
{
    float position[2];
    float color[3];
} vertex_t;

typedef struct _uniform_buffer_object_t
{
    uint32_t model[4][4];
    uint32_t view[4][4];
    uint32_t projection[4][4];
} uniform_buffer_object_t;

struct graphics_s
{
    const char *resource_directory;

    // TODO: instance.h
    SDL_Window *window;
    VkInstance vk_instance;
    VkSurfaceKHR vk_surface;

    // TODO: device.h 
    VkPhysicalDevice vk_physical_device;
    int vk_graphics_queue_index;
    int vk_present_queue_index;
    VkDevice vk_device;
    VkQueue vk_graphics_queue;
    VkQueue vk_present_queue;
    VkSurfaceFormatKHR vk_surface_format;
    VkSwapchainKHR vk_swapchain;
    VkExtent2D vk_swapchain_size;

    // TODO: buffers.h
    VkBuffer vk_index_buffer;
    VkDeviceMemory vk_index_buffer_memory;
    VkBuffer vk_vertex_buffer;
    VkDeviceMemory vk_vertex_buffer_memory;
    VkBuffer *uniform_buffers;
    VkDeviceMemory uniform_buffer_memory;
    void ** uniform_buffer_mappings;

    uint32_t vk_image_count;
    VkImage *vk_images;
    VkImageView *vk_image_views;

    VkRenderPass vk_render_pass;
    VkShaderModule vk_vertex_shader;
    VkShaderModule vk_fragment_shader;
    VkPipeline vk_graphics_pipeline;
    VkFramebuffer *vk_framebuffers;

    VkCommandPool vk_command_pool;
    VkCommandBuffer *vk_command_buffers;

    VkSemaphore vk_image_semaphore;
    VkSemaphore vk_render_semaphore;
    VkFence vk_fence;

    uint32_t vk_current_index;
};
typedef struct graphics_s *graphics_t;

int graphics_create(graphics_t *graphics, const char *const resource_directory);

int graphics_render(graphics_t graphics);

void graphics_destroy(graphics_t graphics);

#endif