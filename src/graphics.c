#include "application.h"

// graphics_create() helper functions
static int graphics_create_window(graphics_t graphics);
static int graphics_create_instance(graphics_t graphics);
static int graphics_create_surface(graphics_t graphics);
static int graphics_create_physical_device(graphics_t graphics);
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
static void graphics_destroy_window(graphics_t graphics);
static void graphics_destroy_instance(graphics_t graphics);
static void graphics_destroy_surface(graphics_t graphics);
static void graphics_destroy_physical_device(graphics_t graphics);
static void graphics_destroy_logical_device(graphics_t graphics);
static void graphics_destroy_semaphores(graphics_t graphics);
static void graphics_destroy_command_pool(graphics_t graphics);
static void graphics_destroy_vertex_buffer(graphics_t graphics);
static void graphics_destroy_uniform_buffer(graphics_t graphics);
static void graphics_destroy_swap_chain(graphics_t graphics);
static void graphics_destroy_render_pass(graphics_t graphics);
static void graphics_destroy_image_views(graphics_t graphics);
static void graphics_destroy_framebuffers(graphics_t graphics);
static void graphics_destroy_graphics_pipeline(graphics_t graphics);
static void graphics_destroy_descriptor_pool(graphics_t graphics);
static void graphics_destroy_descriptor_set(graphics_t graphics);
static void graphics_destroy_command_buffers(graphics_t graphics);

int graphics_create(graphics_t *graphics, const char *const resource_directory)
{
    int status;
    graphics_t new_graphics;

    status = CUBE_SUCCESS;
    new_graphics = NULL;

    new_graphics = calloc(1, sizeof(struct graphics_s));
    if (new_graphics == NULL)
    {
        fputs("graphics_create: failed to allocate graphics\n", stderr);
        goto error;
    }

    if (graphics_create_window(new_graphics) != CUBE_SUCCESS)
    {
        fputs("graphics_create: failed to create window\n", stderr);
        goto error;
    }

    if (graphics_create_instance(new_graphics) != CUBE_SUCCESS)
    {
        fputs("graphics_create: failed to create instance\n", stderr);
        goto error;
    }

    if (graphics_create_surface(new_graphics) != CUBE_SUCCESS)
    {
        fputs("graphics_create: failed to create surface", stderr);
        goto error;
    }

    if (graphics_create_physical_device(new_graphics) != CUBE_SUCCESS)
    {
        fputs("create_graphics: failed to create physical device", stderr);
        goto error;
    }

    *graphics = new_graphics;
    goto done;
error:
    status = CUBE_FAILURE;
    graphics_destroy(new_graphics);
done:
    return status;
}

int graphics_render(graphics_t graphics);

void graphics_destroy(graphics_t graphics)
{
    if (graphics != NULL)
    {
        free(graphics);
    }
}

int graphics_create_window(graphics_t graphics)
{
    int status;
    SDL_DisplayMode display_mode;
    SDL_Window *new_window;

    status = CUBE_SUCCESS;

    if (SDL_GetCurrentDisplayMode(0, &display_mode) < 0)
    {
        fprintf(stderr, "graphics_create_window: SDL_GetCurrentDisplayMode returned error \"%s\"\n", SDL_GetError());
        goto error;
    }

    new_window = SDL_CreateWindow(
        "Johnny's Cube",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        display_mode.w,
        display_mode.h,
        SDL_WINDOW_FULLSCREEN);

    if (new_window == NULL)
    {
        fprintf(stderr, "graphics_create_window: SDL_GetCreateWindow returned error \"%s\"\n", SDL_GetError());
        goto error;
    }

    graphics->window = new_window;

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_instance(graphics_t graphics)
{
    int status;
    size_t instance_extention_count;
    const char **instance_extension_names;
    VkApplicationInfo application_info;
    VkInstanceCreateInfo instance_create_info;
    VkResult create_instance_result;

    status = CUBE_SUCCESS;
    instance_extension_names = NULL;

    if (SDL_Vulkan_GetInstanceExtensions(graphics->window, &instance_extention_count, NULL) < 0)
    {
        fprintf(stderr, "graphics_create_instance: first call to SDL_Vulkan_GetInstanceExtensions returned error \"%s\"\n", SDL_GetError());
        goto error;
    }

    instance_extension_names = (const char **)calloc(instance_extention_count, sizeof(char *));
    if (instance_extension_names == NULL)
    {
        fputs("graphics_create_instance: failed to allocate instance extension names", stderr);
        goto error;
    }

    if (SDL_Vulkan_GetInstanceExtensions(graphics->window, &instance_extention_count, instance_extension_names) < 0)
    {
        fprintf(stderr, "graphics_create_instance: second call to SDL_Vulkan_GetInstanceExtensions returned error \"%s\"\n", SDL_GetError());
        goto error;
    }

    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "Johnny's Cube";
    application_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    application_info.pEngineName = "Johnny's Engine";
    application_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    application_info.pNext = NULL;

    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.ppEnabledLayerNames = NULL;
    instance_create_info.enabledExtensionCount = instance_extention_count;
    instance_create_info.ppEnabledExtensionNames = instance_extension_names;

    create_instance_result = vkCreateInstance(&instance_create_info, NULL, &graphics->vk_instance);
    if (create_instance_result != VK_SUCCESS)
    {
        fprintf(stderr, "create_graphics_instance: vkCreateInstance failed(%d)\n", (int)create_instance_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    if (instance_extension_names != NULL)
    {
        free(instance_extension_names);
    }
    return status;
}

int graphics_create_surface(graphics_t graphics)
{
    int status;

    status = CUBE_SUCCESS;

    if (SDL_Vulkan_CreateSurface(graphics->window, graphics->vk_instance, &graphics->vk_surface) == SDL_FALSE)
    {
        fprintf(stderr, "create_graphics_surface: SDL_Vulkan_CreateSurface returned error \"%s\"\n", SDL_GetError());
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_physical_device(graphics_t graphics)
{
    int status;
    VkResult vk_result;
    size_t physical_device_count;
    VkPhysicalDevice *physical_devices;
    size_t queue_family_property_count;
    VkQueueFamilyProperties *queue_family_properties;
    size_t queue_family_property_index;
    VkQueueFamilyProperties queue_family;
    VkBool32 queue_present_support;

    status = CUBE_SUCCESS;
    physical_devices = NULL;
    queue_family_properties = NULL;
    // TODO: break into local variables for the sake of consistency
    graphics->vk_graphics_queue_index = -1;
    graphics->vk_present_queue_index = -1;
    queue_present_support = VK_FALSE;

    vk_result = vkEnumeratePhysicalDevices(graphics->vk_instance, &physical_device_count, NULL);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_physical_device: first call to vkEnumeratePhysicalDevices failed(%d)\n");
        goto error;
    }

    physical_devices = (VkPhysicalDevice *)calloc(physical_device_count, sizeof(VkPhysicalDevice));
    if (physical_devices == NULL)
    {
        fputs("graphics_create_physical_device: failed to allocate physical devices\n", stderr);
        goto error;
    }

    vk_result = vkEnumeratePhysicalDevices(graphics->vk_instance, &physical_device_count, physical_devices);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_physical_device: second call to vkEnumeratePhysicalDevices failed(%d)\n");
        goto error;
    }

    graphics->vk_physical_device = *physical_devices;

    vkGetPhysicalDeviceQueueFamilyProperties(graphics->vk_physical_device, &queue_family_property_count, NULL);
    queue_family_properties = calloc(queue_family_property_count, sizeof(VkQueueFamilyProperties));
    if (queue_family_properties == NULL)
    {
        fputs("graphics_create_physical_device: failed to allocate queue family properties\n", stderr);
        goto error;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(graphics->vk_physical_device, &queue_family_property_count, queue_family_properties);

    for (queue_family_property_index = 0; queue_family_property_index < queue_family_property_count; queue_family_property_index++)
    {
        queue_family = *(queue_family_properties + queue_family_property_index);
        if (graphics->vk_graphics_queue_index == -1)
        {
            if (queue_family.queueCount > 0 && (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                graphics->vk_graphics_queue_index = queue_family_property_index;
            }
        }

        if (graphics->vk_present_queue_index == -1)
        {
            // TODO: handle result
            vkGetPhysicalDeviceSurfaceSupportKHR(graphics->vk_physical_device, queue_family_property_index, graphics->vk_surface, &queue_present_support);
            if (queue_family.queueCount > 0 && (queue_present_support == VK_TRUE))
            {
                graphics->vk_present_queue_index = queue_family_property_index;
            }
        }
    }

    if (graphics->vk_graphics_queue_index == -1)
    {
        fputs("graphics_create_physical_device: graphics queue unavailable\n", stderr);
        goto error;
    }

    if (graphics->vk_present_queue_index == -1)
    {
        fputs("graphics_create_physical_device: present queue unavailable\n", stderr);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    if (physical_devices != NULL)
    {
        free(physical_devices);
    }

    if (queue_family_properties != NULL)
    {
        free(queue_family_properties);
    }
    return status;
}

int graphics_create_logical_device(graphics_t graphics)
{
    const char *const device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    const float queue_priorities[] = {1.0f};
    const VkPhysicalDeviceFeatures device_features = {.samplerAnisotropy = VK_TRUE};
    int status;
    uint32_t queue_family_indices[2];
    VkResult vk_result;
    VkDeviceQueueCreateInfo queue_create_infos[2];
    VkDeviceCreateInfo device_create_info;

    status = CUBE_SUCCESS;

    queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[0].queueFamilyIndex = graphics->vk_graphics_queue_index;
    queue_create_infos[0].queueCount = 1;
    queue_create_infos[0].pQueuePriorities = &queue_priorities;
    queue_create_infos[0].flags = 0;
    queue_create_infos[0].pNext = NULL;

    queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[1].queueFamilyIndex = graphics->vk_present_queue_index;
    queue_create_infos[1].queueCount = 1;
    queue_create_infos[1].pQueuePriorities = &queue_priorities;
    queue_create_infos[1].flags = 0;
    queue_create_infos[1].pNext = NULL;

    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = (VkDeviceQueueCreateInfo*)queue_create_infos;
    device_create_info.queueCreateInfoCount = 2;

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}