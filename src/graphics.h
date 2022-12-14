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
    VkDevice vk_device;
    VkQueue vk_graphics_queue;
    VkQueue vk_present_queue;

};
typedef struct graphics_s *graphics_t;

int graphics_create(graphics_t *graphics);

int graphics_initialize(graphics_t graphics);

int graphics_render(graphics_t graphics);

void graphics_destroy(graphics_t graphics);

#endif