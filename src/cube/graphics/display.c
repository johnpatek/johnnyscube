#include "cube.h"

static int graphics_create_window(cube_graphics *graphics);
static int graphics_create_instance(cube_graphics *graphics);
static int graphics_create_surface(cube_graphics *graphics);

int graphics_create_display(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(graphics_create_window(graphics) == CUBE_SUCCESS, "failed to create window")
    CUBE_ASSERT(graphics_create_instance(graphics) == CUBE_SUCCESS, "failed to create window")
    CUBE_ASSERT(graphics_create_surface(graphics) == CUBE_SUCCESS, "failed to create window")
    CUBE_END_FUNCTION
}

void graphics_destroy_display(cube_graphics *graphics)
{
    vkDestroySurfaceKHR(graphics->instance, graphics->surface, NULL);
    vkDestroyInstance(graphics->instance, NULL);
    SDL_DestroyWindow(graphics->window);
}

int graphics_create_window(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    SDL_DisplayMode display_mode;

    CUBE_ASSERT(
        SDL_GetCurrentDisplayMode(
            0,
            &display_mode) >= 0,
        SDL_GetError())

    graphics->window = SDL_CreateWindow(
        "Johnny's Cube",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        display_mode.w,
        display_mode.h,
        SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN);
    CUBE_ASSERT(graphics->window != NULL, SDL_GetError())

    graphics->display_size.width = (uint32_t)display_mode.w;
    graphics->display_size.height = (uint32_t)display_mode.h;
    CUBE_END_FUNCTION
}

int graphics_create_instance(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const char *instance_layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    const VkApplicationInfo application_info = {
        .apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 3),
    };
    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
#ifndef NDEBUG
        .enabledLayerCount = sizeof(instance_layers) / sizeof(instance_layers[0]),
        .ppEnabledLayerNames = &instance_layers[0],
#endif
    };

    CUBE_ASSERT(
        SDL_Vulkan_GetInstanceExtensions(
            graphics->window,
            &instance_create_info.enabledExtensionCount,
            NULL) == SDL_TRUE,
        "failed to get instance extension count")

    instance_create_info.ppEnabledExtensionNames = CUBE_CALLOC(
        instance_create_info.enabledExtensionCount,
        sizeof(char *));

    CUBE_ASSERT(
        instance_create_info.ppEnabledExtensionNames != NULL,
        "failed to allocate instance extensions")

    CUBE_ASSERT(
        SDL_Vulkan_GetInstanceExtensions(
            graphics->window,
            &instance_create_info.enabledExtensionCount,
            (const char **)instance_create_info.ppEnabledExtensionNames) == SDL_TRUE,
        "failed to get instance extension names")

    VK_CHECK_RESULT(
        vkCreateInstance(
            &instance_create_info,
            NULL,
            &graphics->instance))
    CUBE_END_FUNCTION
}

int graphics_create_surface(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(
        SDL_Vulkan_CreateSurface(
            graphics->window,
            graphics->instance,
            &graphics->surface) == SDL_TRUE,
        SDL_GetError())
    CUBE_END_FUNCTION
}