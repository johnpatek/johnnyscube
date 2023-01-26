#include "application.h"

#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) \
                                                         : (x))

// graphics_create() helper functions
static int graphics_create_window(graphics_t graphics);
static int graphics_create_instance(graphics_t graphics);
static int graphics_create_surface(graphics_t graphics);
static int graphics_create_physical_device(graphics_t graphics);
static int graphics_create_logical_device(graphics_t graphics);
// TODO: unused
static int graphics_create_vertex_buffer(graphics_t graphics) { return 0; }
// TODO: unused
static int graphics_create_uniform_buffer(graphics_t graphics) { return 0; }
static int graphics_create_swap_chain(graphics_t graphics);
static int graphics_create_image_views(graphics_t graphics);
static int graphics_create_shader_modules(graphics_t graphics);
static int graphics_create_render_pass(graphics_t graphics);
static int graphics_create_pipeline(graphics_t graphics);
static int graphics_create_framebuffers(graphics_t graphics);
static int graphics_create_command_pool(graphics_t graphics);
static int graphics_create_command_buffers(graphics_t graphics);
static int graphics_create_semaphores(graphics_t graphics);
static int graphics_create_fences(graphics_t graphics);
// TODO: unused
static int graphics_create_graphics_pipeline(graphics_t graphics) { return 0; }
// TODO: unused
static int graphics_create_descriptor_pool(graphics_t graphics) { return 0; }
// TODO: unused
static int graphics_create_descriptor_set(graphics_t graphics) { return 0; }

// graphics_render() helper functions;
static int graphics_render_aquire_image(graphics_t graphics);
static int graphics_render_reset_commands(graphics_t graphics);
static int graphics_render_record_commands(graphics_t graphics);
static int graphics_render_queue_submit(graphics_t graphics);
static int graphics_render_queue_present(graphics_t graphics);

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

// misc. utility functions
static int graphics_util_read_file(const char *path, void **data, size_t *size);

int graphics_create(graphics_t *graphics, const char *const resource_directory)
{
    int status;

    status = (graphics != NULL) ? CUBE_SUCCESS : CUBE_FAILURE;
    if (status != CUBE_SUCCESS)
    {
        fputs("graphics_create: invalid graphics handle\n", stderr);
        goto error;
    }

    *graphics = malloc(sizeof(struct graphics_s));
    status = (*graphics != NULL) ? CUBE_SUCCESS : CUBE_FAILURE;
    if (status != CUBE_SUCCESS)
    {
        fputs("graphics_create: failed to allocate graphics\n", stderr);
        goto error;
    }

    (*graphics)->resource_directory = resource_directory;

#define GRAPHICS_CREATE(FUNC)                                 \
    puts(#FUNC);                                              \
    status = FUNC(*graphics);                                 \
    if (status != CUBE_SUCCESS)                               \
    {                                                         \
        fputs("graphics_create: " #FUNC " failed\n", stderr); \
        goto error;                                           \
    }

    GRAPHICS_CREATE(graphics_create_window)
    GRAPHICS_CREATE(graphics_create_instance)
    GRAPHICS_CREATE(graphics_create_surface)
    GRAPHICS_CREATE(graphics_create_physical_device)
    GRAPHICS_CREATE(graphics_create_logical_device)
    GRAPHICS_CREATE(graphics_create_swap_chain)
    GRAPHICS_CREATE(graphics_create_image_views)
    GRAPHICS_CREATE(graphics_create_shader_modules)
    GRAPHICS_CREATE(graphics_create_render_pass)
    GRAPHICS_CREATE(graphics_create_pipeline)
    GRAPHICS_CREATE(graphics_create_framebuffers)
    GRAPHICS_CREATE(graphics_create_command_pool)
    GRAPHICS_CREATE(graphics_create_command_buffers)
    GRAPHICS_CREATE(graphics_create_semaphores)
    GRAPHICS_CREATE(graphics_create_fences)

    goto done;
error:
    fputs("graphics_create: fatal error\n", stderr);
done:
    return status;
}

int graphics_render(graphics_t graphics)
{
    int status;

#define GRAPHICS_RENDER(FUNC)                                 \
    status = FUNC(graphics);                                  \
    if (status != CUBE_SUCCESS)                               \
    {                                                         \
        fputs("graphics_render: " #FUNC " failed\n", stderr); \
        goto error;                                           \
    }

    GRAPHICS_RENDER(graphics_render_aquire_image)
    GRAPHICS_RENDER(graphics_render_reset_commands)
    GRAPHICS_RENDER(graphics_render_record_commands)
    GRAPHICS_RENDER(graphics_render_queue_submit)
    GRAPHICS_RENDER(graphics_render_queue_present)

    goto done;
error:
    fputs("graphics_render: fatal error\n", stderr);
done:
    return status;
}

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
        SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN);

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
    unsigned int instance_extention_count;
    const char **instance_extension_names;
    VkApplicationInfo application_info;
    VkInstanceCreateInfo instance_create_info;
    VkResult create_instance_result;

    status = CUBE_SUCCESS;
    instance_extension_names = NULL;

    if (SDL_Vulkan_GetInstanceExtensions(graphics->window, &instance_extention_count, NULL) != SDL_TRUE)
    {
        fprintf(stderr, "graphics_create_instance: first call to SDL_Vulkan_GetInstanceExtensions returned error \"%s\"\n", SDL_GetError());
        goto error;
    }

    if (instance_extention_count > 0)
    {
        instance_extension_names = calloc(instance_extention_count, sizeof(char *));
        if (instance_extension_names == NULL)
        {
            fputs("graphics_create_instance: failed to allocate instance extension names\n", stderr);
            goto error;
        }

        if (SDL_Vulkan_GetInstanceExtensions(graphics->window, &instance_extention_count, instance_extension_names) != SDL_TRUE)
        {
            fprintf(stderr, "graphics_create_instance: second call to SDL_Vulkan_GetInstanceExtensions returned error \"%s\"\n", SDL_GetError());
            goto error;
        }
    }
    else
    {
        instance_extension_names = NULL;
    }

    SDL_memset(&application_info, 0, sizeof(VkApplicationInfo));
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "Johnny's Cube";
    application_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    application_info.pEngineName = "Johnny's Engine";
    application_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    application_info.pNext = NULL;

    SDL_memset(&instance_create_info, 0, sizeof(VkInstanceCreateInfo));
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
    uint32_t physical_device_count;
    VkPhysicalDevice *physical_devices;
    uint32_t queue_family_property_count;
    VkQueueFamilyProperties *queue_family_properties;
    uint32_t queue_family_property_index;
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
        fprintf(stderr, "graphics_create_physical_device: first call to vkEnumeratePhysicalDevices failed(%d)\n", vk_result);
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
        fprintf(stderr, "graphics_create_physical_device: second call to vkEnumeratePhysicalDevices failed(%d)\n", vk_result);
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
    VkResult vk_result;
    VkDeviceQueueCreateInfo queue_create_infos[2];
    uint32_t unique_queue_indices;
    VkDeviceCreateInfo device_create_info;

    status = CUBE_SUCCESS;
    unique_queue_indices = 1;

    SDL_memset(&queue_create_infos[0], 0, sizeof(VkDeviceQueueCreateInfo));
    queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[0].queueFamilyIndex = graphics->vk_graphics_queue_index;
    queue_create_infos[0].queueCount = 1;
    queue_create_infos[0].pQueuePriorities = &queue_priorities[0];
    queue_create_infos[0].flags = 0;
    queue_create_infos[0].pNext = NULL;
    if (graphics->vk_graphics_queue_index != graphics->vk_present_queue_index)
    {
        SDL_memset(&queue_create_infos[1], 0, sizeof(VkDeviceQueueCreateInfo));
        queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[1].queueFamilyIndex = graphics->vk_present_queue_index;
        queue_create_infos[1].queueCount = 1;
        queue_create_infos[1].pQueuePriorities = &queue_priorities[0];
        queue_create_infos[1].flags = 0;
        queue_create_infos[1].pNext = NULL;
        unique_queue_indices++;
    }
    SDL_memset(&device_create_info, 0, sizeof(VkDeviceCreateInfo));
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = (VkDeviceQueueCreateInfo *)queue_create_infos;
    device_create_info.queueCreateInfoCount = unique_queue_indices;
    device_create_info.ppEnabledExtensionNames = device_extensions;
    device_create_info.enabledExtensionCount = 1;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.pNext = NULL;
    device_create_info.ppEnabledLayerNames = NULL;
    device_create_info.enabledLayerCount = 0;

    vk_result = vkCreateDevice(graphics->vk_physical_device, &device_create_info, NULL, &graphics->vk_device);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_logical_device: vkCreateDevice failed(%d)\n", vk_result);
        goto error;
    }

    vkGetDeviceQueue(graphics->vk_device, graphics->vk_graphics_queue_index, 0, &graphics->vk_graphics_queue);

    vkGetDeviceQueue(graphics->vk_device, graphics->vk_present_queue_index, 0, &graphics->vk_present_queue);

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_swap_chain(graphics_t graphics)
{
    int status;
    VkResult vk_result;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    uint32_t surface_format_count;
    VkSurfaceFormatKHR *surface_formats;
    int drawable_width;
    int drawable_height;
    VkExtent2D swapchain_size;
    uint32_t image_count;
    VkSwapchainCreateInfoKHR swapchain_create_info;

    status = CUBE_SUCCESS;
    surface_formats = NULL;

    vk_result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphics->vk_physical_device, graphics->vk_surface, &surface_capabilities);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swap_chain: vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed(%d)\n", vk_result);
        goto error;
    }

    vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(graphics->vk_physical_device, graphics->vk_surface, &surface_format_count, NULL);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swap_chain: first call to vkGetPhysicalDeviceSurfaceFormatsKHR failed(%d)\n", vk_result);
        goto error;
    }

    surface_formats = calloc(surface_format_count, sizeof(VkSurfaceFormatKHR));
    if (surface_formats == NULL)
    {
        fputs("graphics_create_swap_chain: failed to allocate surface formats\n", stderr);
        goto error;
    }

    vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(graphics->vk_physical_device, graphics->vk_surface, &surface_format_count, surface_formats);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swap_chain: second call to vkGetPhysicalDeviceSurfaceFormatsKHR failed(%d)\n", vk_result);
        goto error;
    }

    if (surface_formats[0].format != VK_FORMAT_B8G8R8A8_UNORM)
    {
        fputs("graphics_create_swap_chain: surface_formats[0].format != VK_FORMAT_B8G8R8A8_UNORM\n", stderr);
        goto error;
    }

    graphics->vk_surface_format = *surface_formats;

    SDL_Vulkan_GetDrawableSize(graphics->window, &drawable_width, &drawable_height);
    drawable_width = CLAMP((uint32_t)drawable_width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
    drawable_height = CLAMP((uint32_t)drawable_height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);

    swapchain_size.width = drawable_width;
    swapchain_size.height = drawable_height;

    image_count = CLAMP(surface_capabilities.minImageCount + 1, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);

    SDL_memset(&swapchain_create_info, 0, sizeof(VkSwapchainCreateInfoKHR));
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = graphics->vk_surface;
    swapchain_create_info.minImageCount = surface_capabilities.minImageCount;
    swapchain_create_info.imageFormat = graphics->vk_surface_format.format;
    swapchain_create_info.imageColorSpace = graphics->vk_surface_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_size;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (graphics->vk_graphics_queue_index != graphics->vk_present_queue_index)
    {
        uint32_t queue_family_indices[] = {graphics->vk_graphics_queue_index, graphics->vk_present_queue_index};
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapchain_create_info.preTransform = surface_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchain_create_info.clipped = VK_TRUE;

    vk_result = vkCreateSwapchainKHR(graphics->vk_device, &swapchain_create_info, NULL, &graphics->vk_swapchain);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swap_chain: vkCreateSwapchainKHR failed(%d)\n", vk_result);
        goto error;
    }

    vk_result = vkGetSwapchainImagesKHR(graphics->vk_device, graphics->vk_swapchain, &graphics->vk_image_count, NULL);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swap_chain: first call to vkGetSwapchainImagesKHR failed(%d)\n", vk_result);
        goto error;
    }

    graphics->vk_images = calloc(graphics->vk_image_count, sizeof(VkImage));
    if (graphics->vk_images == NULL)
    {
        fputs("graphics_create_swap_chain: failed to allocate images\n", stderr);
        goto error;
    }

    vk_result = vkGetSwapchainImagesKHR(graphics->vk_device, graphics->vk_swapchain, &graphics->vk_image_count, graphics->vk_images);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swap_chain: second call to vkGetSwapchainImagesKHR failed(%d)\n", vk_result);
        goto error;
    }

    graphics->vk_swapchain_size.width = swapchain_size.width;
    graphics->vk_swapchain_size.height = swapchain_size.height;

    goto done;
error:
    status = CUBE_FAILURE;
done:
    if (surface_formats != NULL)
    {
        free(surface_formats);
    }

    return status;
}

int graphics_create_image_views(graphics_t graphics)
{
    int status;
    uint32_t image_index;
    VkResult vk_result;

    status = CUBE_SUCCESS;

    graphics->vk_image_views = calloc(graphics->vk_image_count, sizeof(VkImageView));
    if (graphics->vk_image_views == NULL)
    {
        fputs("graphics_create_image_views: failed to allocate image views\n", stderr);
        goto error;
    }

    for (image_index = 0; image_index < graphics->vk_image_count; image_index++)
    {
        VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = graphics->vk_images[image_index],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = graphics->vk_surface_format.format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vk_result = vkCreateImageView(
            graphics->vk_device,
            &image_view_create_info,
            NULL,
            &graphics->vk_image_views[image_index]);
        if (vk_result != VK_SUCCESS)
        {
            fprintf(stderr, "graphics_create_image_views: vkCreateImageView failed(%d)\n", vk_result);
            goto error;
        }
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_shader_modules(graphics_t graphics)
{
    int status;
    char vertex_shader_path[256];
    void *vertex_shader_code;
    size_t vertex_shader_size;
    char fragment_shader_path[256];
    void *fragment_shader_code;
    size_t fragment_shader_size;
    VkResult vk_result;

    status = CUBE_SUCCESS;
    vertex_shader_code = NULL;
    fragment_shader_code = NULL;

    sprintf(
        vertex_shader_path,
        "%s%s%s%s%s",
        graphics->resource_directory,
        PATH_SEPARATOR,
        "shaders",
        PATH_SEPARATOR,
        "vert.spv");

    sprintf(
        fragment_shader_path,
        "%s%s%s%s%s",
        graphics->resource_directory,
        PATH_SEPARATOR,
        "shaders",
        PATH_SEPARATOR,
        "frag.spv");

    if (graphics_util_read_file(
            vertex_shader_path,
            &vertex_shader_code,
            &vertex_shader_size) != CUBE_SUCCESS)
    {
        fputs("graphics_create_shader_modules: first call to graphics_util_read_file() failed\n", stderr);
        goto error;
    }

    if (graphics_util_read_file(
            fragment_shader_path,
            &fragment_shader_code,
            &fragment_shader_size) != CUBE_SUCCESS)
    {
        fputs("graphics_create_shader_modules: second call to graphics_util_read_file() failed\n", stderr);
        goto error;
    }

    VkShaderModuleCreateInfo vertex_shader_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pCode = (const uint32_t *)vertex_shader_code,
        .codeSize = vertex_shader_size,
        .flags = 0,
        .pNext = NULL,
    };

    VkShaderModuleCreateInfo fragment_shader_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pCode = (const uint32_t *)fragment_shader_code,
        .codeSize = fragment_shader_size,
        .flags = 0,
        .pNext = NULL,
    };

    vk_result = vkCreateShaderModule(
        graphics->vk_device,
        &vertex_shader_create_info,
        NULL,
        &graphics->vk_vertex_shader);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(
            stderr,
            "graphics_create_shader_modules: first call to vkCreateShaderModule failed(%d)\n",
            vk_result);
        goto error;
    }

    vk_result = vkCreateShaderModule(
        graphics->vk_device,
        &fragment_shader_create_info,
        NULL,
        &graphics->vk_fragment_shader);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(
            stderr,
            "graphics_create_shader_modules: second call to vkCreateShaderModule failed(%d)\n",
            vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    if (vertex_shader_code != NULL)
    {
        free(vertex_shader_code);
    }
    if (fragment_shader_code != NULL)
    {
        free(fragment_shader_code);
    }
    return status;
}

int graphics_create_render_pass(graphics_t graphics)
{
    int status;
    VkResult vk_result;
    VkAttachmentDescription attachments[] = {
        // color attachment
        {
            .format = graphics->vk_surface_format.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
    };

    VkAttachmentReference color_reference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass_description = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_reference,
        .pDepthStencilAttachment = NULL,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
        .pResolveAttachments = NULL,
    };

    VkSubpassDependency subpass_dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    };

    VkRenderPassCreateInfo render_pass_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 2,
        .pAttachments = (VkAttachmentDescription *)attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 1,
        .pDependencies = &subpass_dependency,
    };

    status = CUBE_SUCCESS;

    vk_result = vkCreateRenderPass(
        graphics->vk_device,
        &render_pass_create_info,
        NULL,
        &graphics->vk_render_pass);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_render_pass: vkCreateRenderPass failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_pipeline(graphics_t graphics)
{
    VkPipelineShaderStageCreateInfo shader_stages[] = {
        // vertex shader
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = graphics->vk_vertex_shader,
            .pName = "main",
        },
        // fragment shader
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = graphics->vk_fragment_shader,
            .pName = "main",
        },
    };
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .vertexAttributeDescriptionCount = 0,
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };
    VkPipelineRasterizationStateCreateInfo rasterization_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
    };
    VkPipelineMultisampleStateCreateInfo multisample_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };
    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
    };
    VkPipelineColorBlendStateCreateInfo color_blend_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
    };
    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamic_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = &dynamic_states[0],
    };
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0,
    };
    int status;
    VkResult vk_result;
    VkPipelineLayout pipeline_layout;

    status = CUBE_SUCCESS;

    vk_result = vkCreatePipelineLayout(
        graphics->vk_device,
        &pipeline_layout_info,
        NULL,
        &pipeline_layout);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_pipeline: vkCreatePipelineLayout failed(%d)\n", vk_result);
        goto error;
    }

    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly_info,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterization_state_info,
        .pMultisampleState = &multisample_state_info,
        .pColorBlendState = &color_blend_state_info,
        .pDynamicState = &dynamic_state_info,
        .layout = pipeline_layout,
        .renderPass = graphics->vk_render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
    };

    vk_result = vkCreateGraphicsPipelines(
        graphics->vk_device,
        VK_NULL_HANDLE,
        1,
        &pipeline_create_info,
        NULL,
        &graphics->vk_graphics_pipeline);

    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_pipeline: vkCreatePipelines failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    vkDestroyShaderModule(graphics->vk_device, graphics->vk_fragment_shader, NULL);
    vkDestroyShaderModule(graphics->vk_device, graphics->vk_vertex_shader, NULL);
    return status;
}

int graphics_create_framebuffers(graphics_t graphics)
{
    int status;
    uint32_t frame_buffer_index;
    VkResult vk_result;

    status = CUBE_SUCCESS;

    graphics->vk_framebuffers = calloc(graphics->vk_image_count, sizeof(VkFramebuffer));
    if (graphics->vk_framebuffers == NULL)
    {
        fputs("graphics_create_framebuffers: failed to allocate framebuffers\n", stderr);
        goto error;
    }

    for (frame_buffer_index = 0; frame_buffer_index < graphics->vk_image_count; frame_buffer_index++)
    {
        VkImageView frame_buffer_attachments[] = {
            graphics->vk_image_views[frame_buffer_index],
        };

        VkFramebufferCreateInfo frame_buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = graphics->vk_render_pass,
            .attachmentCount = 1,
            .pAttachments = (VkImageView *)frame_buffer_attachments,
            .width = graphics->vk_swapchain_size.width,
            .height = graphics->vk_swapchain_size.height,
            .layers = 1,
        };

        vk_result = vkCreateFramebuffer(
            graphics->vk_device,
            &frame_buffer_create_info,
            NULL,
            &graphics->vk_framebuffers[frame_buffer_index]);
        if (vk_result != VK_SUCCESS)
        {
            fprintf(stderr, "graphics_create_framebuffers: vkCreateFramebuffer failed(%d)\n", vk_result);
            goto error;
        }
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_command_pool(graphics_t graphics)
{
    int status;
    VkResult vk_result;
    VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = graphics->vk_graphics_queue_index,
    };

    status = CUBE_SUCCESS;
    vk_result = vkCreateCommandPool(graphics->vk_device, &command_pool_create_info, NULL, &graphics->vk_command_pool);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_command_pool: vkCreateCommandPool failed(%d)\n", vk_result);
        goto error;
    }
    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_command_buffers(graphics_t graphics)
{
    int status;
    VkResult vk_result;
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = graphics->vk_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = graphics->vk_image_count,
    };

    status = CUBE_SUCCESS;

    graphics->vk_command_buffers = calloc(graphics->vk_image_count, sizeof(VkCommandBuffer));
    if (graphics->vk_command_buffers == NULL)
    {
        fputs("graphics_create_command_buffers: failed to allocate command buffers\n", stderr);
        goto error;
    }

    vk_result = vkAllocateCommandBuffers(
        graphics->vk_device,
        &command_buffer_allocate_info,
        graphics->vk_command_buffers);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_command_buffers: vkAllocateCommandBuffers failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_semaphores(graphics_t graphics)
{
    int status;
    VkResult vk_result;
    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };

    status = CUBE_SUCCESS;

    vk_result = vkCreateSemaphore(
        graphics->vk_device,
        &semaphore_create_info,
        NULL,
        &graphics->vk_image_semaphore);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_command_buffers: first call to vkAllocateCommandBuffers failed(%d)\n", vk_result);
        goto error;
    }

    vk_result = vkCreateSemaphore(
        graphics->vk_device,
        &semaphore_create_info,
        NULL,
        &graphics->vk_render_semaphore);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_command_buffers: second call to vkAllocateCommandBuffers failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_create_fences(graphics_t graphics)
{
    int status;
    uint32_t fence_index;
    VkResult vk_result;
    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    status = CUBE_SUCCESS;

    graphics->vk_fences = calloc(graphics->vk_image_count, sizeof(VkFence));
    if (graphics->vk_fences == NULL)
    {
        fputs("graphics_create_fences: failed to allocate fences", stderr);
        goto error;
    }

    for (fence_index = 0; fence_index < graphics->vk_image_count; fence_index++)
    {
        vk_result = vkCreateFence(
            graphics->vk_device,
            &fence_create_info,
            NULL,
            &graphics->vk_fences[fence_index]);
        if (vk_result != VK_SUCCESS)
        {
            fprintf(stderr, "graphics_create_fences: vkCreateFence failed(%d)\n", vk_result);
            goto error;
        }
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_render_aquire_image(graphics_t graphics)
{
    int status;
    VkResult vk_result;

    status = CUBE_SUCCESS;

    vk_result = vkAcquireNextImageKHR(
        graphics->vk_device,
        graphics->vk_swapchain,
        UINT64_MAX,
        graphics->vk_image_semaphore,
        VK_NULL_HANDLE,
        &graphics->vk_current_index);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_render_aquire_image: vkAcquireNextImageKHR failed(%d)\n", vk_result);
        goto error;
    }

    vk_result = vkWaitForFences(
        graphics->vk_device,
        1,
        &graphics->vk_fences[graphics->vk_current_index],
        VK_FALSE, UINT64_MAX);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_render_aquire_image: vkWaitForFences failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_render_reset_commands(graphics_t graphics)
{
    const VkCommandBufferResetFlags reset_flags = 0;
    int status;
    VkCommandBuffer command_buffer;
    VkResult vk_result;

    status = CUBE_SUCCESS;
    command_buffer = graphics->vk_command_buffers[graphics->vk_current_index];

    vk_result = vkResetCommandBuffer(command_buffer, reset_flags);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_render_reset_commands: vkResetCommandBuffer failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_render_record_commands(graphics_t graphics)
{
    const VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        .pNext = NULL,
        .pInheritanceInfo = NULL,
    };
    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .clearValueCount = 1,
        .pClearValues = &clear_color,
        .framebuffer = graphics->vk_framebuffers[graphics->vk_current_index],
        .renderPass = graphics->vk_render_pass,
        .renderArea = {
            .extent = graphics->vk_swapchain_size,
            .offset = {0, 0},
        },
    };
    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)graphics->vk_swapchain_size.width,
        .height = (float)graphics->vk_swapchain_size.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = graphics->vk_swapchain_size,
    };
    int status;
    VkCommandBuffer command_buffer;
    VkResult vk_result;

    status = CUBE_SUCCESS;

    command_buffer = graphics->vk_command_buffers[graphics->vk_current_index];

    vk_result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(
            stderr,
            "graphics_render_record_commands: vkBeginCommandBuffer failed(%d)\n",
            vk_result);
        goto error;
    }

    vkCmdBeginRenderPass(
        command_buffer,
        &render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphics->vk_graphics_pipeline);
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    vkCmdDraw(command_buffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(command_buffer);

    vk_result = vkEndCommandBuffer(command_buffer);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(
            stderr,
            "graphics_render_record_commands: vkEndCommandBuffer failed(%d)\n",
            vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_render_queue_submit(graphics_t graphics)
{
    const VkPipelineStageFlags wait_dest_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    int status;
    VkCommandBuffer command_buffer;
    VkResult vk_result;

    status = CUBE_SUCCESS;
    command_buffer = graphics->vk_command_buffers[graphics->vk_current_index];

    VkSubmitInfo queue_submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &graphics->vk_render_semaphore,
        .pWaitDstStageMask = &wait_dest_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &graphics->vk_image_semaphore,
    };

    vk_result = vkQueueSubmit(
        graphics->vk_graphics_queue,
        1,
        &queue_submit_info,
        graphics->vk_fences[graphics->vk_current_index]);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_render_queue_submit: vkQueueSubmit failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_render_queue_present(graphics_t graphics)
{
    const VkPipelineStageFlags wait_dest_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    int status;
    VkCommandBuffer command_buffer;
    VkResult vk_result;

    status = CUBE_SUCCESS;
    command_buffer = graphics->vk_command_buffers[graphics->vk_current_index];

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &graphics->vk_image_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &graphics->vk_swapchain,
        .pImageIndices = &graphics->vk_current_index,
    };

    vk_result = vkQueuePresentKHR(graphics->vk_present_queue, &present_info);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_render_queue_present: vkQueuePresentKHR failed(%d)\n", vk_result);
        goto error;
    }

    vk_result = vkQueueWaitIdle(graphics->vk_present_queue);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_render_queue_present: vkQueueWaitIdle failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_util_read_file(const char *path, void **data, size_t *size)
{
    int status;
    FILE *file;

    status = CUBE_SUCCESS;
    file = NULL;
    *data = NULL;
    *size = 0;

#if defined(_WIN32)
    fopen_s(&file, path, "r");
#else
    file = fopen(path, "r");
#endif
    if (file == NULL)
    {
        fputs("graphics_util_read_file: fopen() failed\n", stderr);
        goto error;
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fputs("graphics_util_read_file: first fseek() failed\n", stderr);
        goto error;
    }

    *size = (size_t)ftell(file);
    *data = malloc(*size);
    if (*data == NULL)
    {
        fputs("graphics_util_read_file: malloc() failed\n", stderr);
        goto error;
    }

    if (fseek(file, 0, SEEK_SET) != 0)
    {
        fputs("graphics_util_read_file: second fseek() failed\n", stderr);
        goto error;
    }

    fread(*data, 1, *size, file);

    goto done;
error:
    status = CUBE_FAILURE;
    if (data != NULL)
    {
        free(data);
    }
done:
    if (file != NULL)
    {
        fclose(file);
    }
    return status;
}
