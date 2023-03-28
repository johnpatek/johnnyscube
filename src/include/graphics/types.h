#ifndef CUBE_GRAPHICS_TYPES_H
#define CUBE_GRAPHICS_TYPES_H

#include "application/common.h"

typedef struct _cube_vertex
{
    float position[2];
    float color[3];
} cube_vertex;

typedef struct _cube_object
{
    VkBuffer vertex_buffer;
    VkBuffer index_buffer;
    VmaAllocation vertex_buffer_allocation;
    VmaAllocation index_buffer_allocation;
    uint32_t vertex_count;
    uint32_t index_count;
} cube_object;

typedef struct _cube_frame
{
    uint32_t index;
    VkImageView image_view;
    VkFramebuffer framebuffer;
    VkCommandBuffer command_buffer;
    VkFence fence;
    VkBuffer uniform_buffer;
    VmaAllocation uniform_buffer_allocation;
    void *uniform_buffer_mapping;
    VkDescriptorSet descriptor_set;
} cube_frame;

typedef struct _cube_ubo
{
    float model[4][4];
    float view[4][4];
    float projection[4][4];
} cube_ubo;

typedef struct _cube_graphics
{
    char *shader_directory;

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
    VmaAllocator allocator;
    VkCommandPool command_pool;

    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSurfaceFormatKHR surface_format;
    VkRenderPass render_pass;
    VkDescriptorSetLayout descriptor_set_layout;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;

    cube_object *object;
    clock_t timestamp;

    VkSwapchainKHR swapchain;
    uint32_t frame_count;
    VkDescriptorPool descriptor_pool;
    cube_frame *frames;
    VkDescriptorSet *descriptor_sets;
    VkSemaphore frame_rendered;
    VkSemaphore frame_presented;
    VkFence command_fence;
} cube_graphics;

#endif