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
static int graphics_create_depth_stencil(graphics_t graphics);
static int graphics_create_render_pass(graphics_t graphics);
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
static int graphics_render_begin_commands(graphics_t graphics);
static int graphics_render_begin_pass(graphics_t graphics);
static int graphics_render_end_pass(graphics_t graphics);
static int graphics_render_end_commands(graphics_t graphics);
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

int graphics_create(graphics_t *graphics, const char *const resource_directory)
{
    int status;

    status = (graphics != NULL) ? CUBE_SUCCESS : CUBE_FAILURE;
    if (status != CUBE_SUCCESS)
    {
        fputs("graphics_create: inva!= SDL_TRUElid graphics handle\n", stderr);
        goto error;
    }

    *graphics = malloc(sizeof(struct graphics_s));
    status = (*graphics != NULL) ? CUBE_SUCCESS : CUBE_FAILURE;
    if (status != CUBE_SUCCESS)
    {
        fputs("graphics_create: failed to allocate graphics\n", stderr);
        goto error;
    }

#define GRAPHICS_CREATE(FUNC)                                 \
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
    GRAPHICS_CREATE(graphics_create_depth_stencil)
    GRAPHICS_CREATE(graphics_create_render_pass)
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
    GRAPHICS_RENDER(graphics_render_begin_commands)
    GRAPHICS_RENDER(graphics_render_begin_pass)
    GRAPHICS_RENDER(graphics_render_end_pass)
    GRAPHICS_RENDER(graphics_render_end_commands)
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
    if(graphics->vk_graphics_queue_index != graphics->vk_present_queue_index)
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

int graphics_create_depth_stencil(graphics_t graphics)
{
    const VkFormat depth_formats[] = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM,
    };
    const int depth_format_count = sizeof(depth_formats) / sizeof(depth_formats[0]);

    int status;
    int depth_format_index;
    VkFormatProperties format_properties;
    VkFormat depth_format;
    VkImageCreateInfo depth_image_create_info;
    VkResult vk_result;
    VkMemoryRequirements memory_requirements;
    VkPhysicalDeviceMemoryProperties memory_properties;
    uint32_t memory_type_index;
    uint32_t memory_type;
    VkMemoryAllocateInfo memory_allocate_info;
    VkImageViewCreateInfo depth_image_view_create_info;

    status = CUBE_SUCCESS;
    depth_format = VK_FORMAT_UNDEFINED;

    depth_format_index = 0;
    while ((depth_format == VK_FORMAT_UNDEFINED) && (depth_format_index < depth_format_count))
    {
        vkGetPhysicalDeviceFormatProperties(
            graphics->vk_physical_device,
            depth_formats[depth_format_index],
            &format_properties);
        if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            depth_format = depth_formats[depth_format_index];
        }
        else
        {
            depth_format_index++;
        }
    }

    if (depth_format == VK_FORMAT_UNDEFINED)
    {
        fputs("graphics_create_depth_stencil: failed to find supported depth format\n", stderr);
        goto error;
    }

    graphics->vk_depth_format = depth_format;

    SDL_memset(&depth_image_create_info, 0, sizeof(VkImageCreateInfo));
    depth_image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depth_image_create_info.imageType = VK_IMAGE_TYPE_2D;
    depth_image_create_info.mipLevels = 1;
    depth_image_create_info.arrayLayers = 1;
    depth_image_create_info.format = graphics->vk_depth_format;
    depth_image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    depth_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depth_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    depth_image_create_info.extent.width = graphics->vk_swapchain_size.width;
    depth_image_create_info.extent.height = graphics->vk_swapchain_size.height;
    depth_image_create_info.extent.depth = 1;

    vk_result = vkCreateImage(
        graphics->vk_device,
        &depth_image_create_info,
        NULL,
        &graphics->vk_depth_image);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_depth_stencil: vkCreateImage failed(%d)\n", vk_result);
        goto error;
    }

    vkGetImageMemoryRequirements(
        graphics->vk_device,
        graphics->vk_depth_image,
        &memory_requirements);

    vkGetPhysicalDeviceMemoryProperties(graphics->vk_physical_device, &memory_properties);

    memory_type = memory_properties.memoryTypeCount;
    memory_type_index = 0;
    while ((memory_type == memory_properties.memoryTypeCount) && (memory_type_index < memory_properties.memoryTypeCount))
    {
        if (((memory_type_index & memory_requirements.memoryTypeBits) != 0) && ((memory_properties.memoryTypes[memory_type_index].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
        {
            memory_type = memory_type_index;
        }
        else
        {
            memory_type_index++;
        }
    }

    if (memory_type_index == memory_properties.memoryTypeCount)
    {
        fputs("graphics_create_depth_stencil: failed to find memory type\n", stderr);
        goto error;
    }

    SDL_memset(&memory_allocate_info, 0, sizeof(VkMemoryAllocateInfo));
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;

    vk_result = vkAllocateMemory(
        graphics->vk_device,
        &memory_allocate_info,
        NULL,
        &graphics->vk_depth_image_memory);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_depth_stencil: vkAllocateMemory failed(%d)\n", vk_result);
        goto error;
    }

    vkBindImageMemory(
        graphics->vk_device,
        graphics->vk_depth_image,
        graphics->vk_depth_image_memory,
        0);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_depth_stencil: vkBindImageMemory failed(%d)\n", vk_result);
        goto error;
    }

    SDL_memset(&depth_image_view_create_info, 0, sizeof(VkImageViewCreateInfo));
    depth_image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depth_image_view_create_info.image = graphics->vk_depth_image;
    depth_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depth_image_view_create_info.format = graphics->vk_depth_format;
    depth_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth_image_view_create_info.subresourceRange.baseMipLevel = 0;
    depth_image_view_create_info.subresourceRange.levelCount = 1;
    depth_image_view_create_info.subresourceRange.baseArrayLayer = 0;
    depth_image_view_create_info.subresourceRange.layerCount = 1;

    vk_result = vkCreateImageView(
        graphics->vk_device,
        &depth_image_view_create_info,
        NULL,
        &graphics->vk_depth_image_view);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_depth_stencil: vkCreateImageView failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
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
        // depth attachment
        {
            .format = graphics->vk_depth_format,
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

    VkAttachmentReference depth_reference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass_description = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_reference,
        .pDepthStencilAttachment = &depth_reference,
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
            graphics->vk_depth_image_view,
        };

        VkFramebufferCreateInfo frame_buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = graphics->vk_render_pass,
            .attachmentCount = 2,
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

int graphics_render_begin_commands(graphics_t graphics)
{
    const VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };
    int status;
    VkCommandBuffer command_buffer;
    VkResult vk_result;

    status = CUBE_SUCCESS;
    command_buffer = graphics->vk_command_buffers[graphics->vk_current_index];

    vk_result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_render_begin_commands: vkBeginCommandBuffer failed(%d)\n", vk_result);
        goto error;
    }

    goto done;
error:
    status = CUBE_FAILURE;
done:
    return status;
}

int graphics_render_begin_pass(graphics_t graphics)
{
    const VkClearValue render_pass_clear_values[] = {
        {
            .color = {0.0f, 0.0f, 0.0f, 1.0f},
            .depthStencil = {1.0f, 0},
        },
    };
    const VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = graphics->vk_render_pass,
        .renderArea = {
            .offset = {0, 0},
            .extent = graphics->vk_swapchain_size,
        },
        .clearValueCount = 1,
        .pClearValues = (VkClearValue *)render_pass_clear_values,
    };
    const VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };
    int status;
    VkCommandBuffer command_buffer;

    status = CUBE_SUCCESS;
    command_buffer = graphics->vk_command_buffers[graphics->vk_current_index];

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    return status;
}

int graphics_render_end_pass(graphics_t graphics)
{
    int status;
    VkCommandBuffer command_buffer;

    status = CUBE_SUCCESS;
    command_buffer = graphics->vk_command_buffers[graphics->vk_current_index];

    vkCmdEndRenderPass(command_buffer);

    return status;
}

int graphics_render_end_commands(graphics_t graphics)
{
    int status;
    VkCommandBuffer command_buffer;
    VkResult vk_result;

    status = CUBE_SUCCESS;
    command_buffer = graphics->vk_command_buffers[graphics->vk_current_index];

    vk_result = vkEndCommandBuffer(command_buffer);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_render_end_commands: vkEndCommandBuffer failed(%d)\n", vk_result);
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