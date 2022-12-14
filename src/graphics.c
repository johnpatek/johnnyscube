#include "application.h"

// graphics_create() helper functions
static int graphics_create_instance(graphics_t graphics);
static int graphics_create_window(graphics_t graphics);
static int graphics_create_surface(graphics_t graphics);
static int graphics_create_physical_device(graphics_t graphics);
static int graphics_create_queues(graphics_t graphics);
static int graphics_create_logical_device(graphics_t graphics);
static int graphics_create_semaphores(graphics_t graphics);
static int graphics_create_command_pool(graphics_t graphics);
static int graphics_create_vertex_buffer(graphics_t graphics);
static int graphics_create_uniform_buffer(graphics_t graphics);
static int graphics_create_swap_chain(graphics_t graphics);
static int graphics_create_render_pass(graphics_t graphics);
static int graphics_create_image_views(graphics_t graphics);
static int graphics_create_framebuffers(graphics_t graphics);
static int graphics_create_graphics_pipeline(graphics_t graphics);
static int graphics_create_descriptor_pool(graphics_t graphics);
static int graphics_create_descriptor_set(graphics_t graphics);
static int graphics_create_command_buffers(graphics_t graphics);

// graphics_destroy() helper functions
static int graphics_destroy_instance(graphics_t graphics);
static int graphics_destroy_window(graphics_t graphics);
static int graphics_destroy_surface(graphics_t graphics);
static int graphics_destroy_physical_device(graphics_t graphics);
static int graphics_destroy_queues(graphics_t graphics);
static int graphics_destroy_logical_device(graphics_t graphics);
static int graphics_destroy_semaphores(graphics_t graphics);
static int graphics_destroy_command_pool(graphics_t graphics);
static int graphics_destroy_vertex_buffer(graphics_t graphics);
static int graphics_destroy_uniform_buffer(graphics_t graphics);
static int graphics_destroy_swap_chain(graphics_t graphics);
static int graphics_destroy_render_pass(graphics_t graphics);
static int graphics_destroy_image_views(graphics_t graphics);
static int graphics_destroy_framebuffers(graphics_t graphics);
static int graphics_destroy_graphics_pipeline(graphics_t graphics);
static int graphics_destroy_descriptor_pool(graphics_t graphics);
static int graphics_destroy_descriptor_set(graphics_t graphics);
static int graphics_destroy_command_buffers(graphics_t graphics);

static int graphics_rate_physical_device(VkPhysicalDevice physical_device);

int graphics_create(graphics_t *graphics)
{
    *graphics = calloc(1, sizeof(struct graphics_s));

    return 0;
}

void graphics_destroy(graphics_t graphics)
{
    if (graphics != NULL)
    {
        vkDestroySurfaceKHR(graphics->vk_instance, graphics->vk_surface, NULL);
        vkDestroyInstance(graphics->vk_instance, NULL);
        SDL_DestroyWindow(graphics->window);
        free(graphics);
    }
    SDL_Vulkan_UnloadLibrary();
}

int graphics_initialize(graphics_t graphics)
{
    const char **instance_extension_names;
    VkPhysicalDevice *physical_devices;

    size_t instance_extension_count;
    size_t physical_device_count;
    size_t physical_device_index;

    instance_extension_names = NULL;

    SDL_Vulkan_LoadLibrary(NULL);

    SDL_GetCurrentDisplayMode(0, &graphics->display_mode);

    graphics->window = SDL_CreateWindow(
        "Johnny's Cube",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        graphics->display_mode.w,
        graphics->display_mode.h,
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_VULKAN);

    SDL_Vulkan_GetInstanceExtensions(graphics->window, &instance_extension_count, NULL);
    if (instance_extension_count > 0)
    {
        instance_extension_names = (const char **)calloc(instance_extension_count, sizeof(char *));
        SDL_Vulkan_GetInstanceExtensions(
            graphics->window,
            &instance_extension_count,
            instance_extension_names);
    }

    const VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = NULL,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = instance_extension_count,
        .ppEnabledExtensionNames = instance_extension_names,
    };

    vkCreateInstance(
        &instance_create_info,
        NULL,
        &graphics->vk_instance);

    free(instance_extension_names);

    vkEnumeratePhysicalDevices(graphics->vk_instance, &physical_device_count, NULL);

    physical_devices = (VkPhysicalDevice *)calloc(physical_device_count, sizeof(VkPhysicalDevice));

    vkEnumeratePhysicalDevices(graphics->vk_instance, &physical_device_count, physical_devices);

    for (physical_device_index = 0; physical_device_index < physical_device_count; physical_device_index++)
    {
        if (graphics->vk_physical_device == NULL)
        {
            if (graphics_rate_physical_device(*(physical_devices + physical_device_index)) > 0)
            {
                puts("found GPU");
                graphics->vk_physical_device = *(physical_devices + physical_device_index);
            }
        }
    }

    if (graphics->vk_physical_device == NULL)
    {
        puts("no GPU detected, using CPU");
        graphics->vk_physical_device = *physical_devices;
    }

    free(physical_devices);

    SDL_Vulkan_CreateSurface(
        graphics->window,
        graphics->vk_instance,
        &graphics->vk_surface);

    return 0;
}

int graphics_render(graphics_t graphics)
{

    return 0;
}

static int graphics_rate_physical_device(VkPhysicalDevice physical_device)
{
    int rating;
    VkPhysicalDeviceProperties physical_device_properties;

    rating = 0;
    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

    if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        rating = 1;
    }

    return rating;
}