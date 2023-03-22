#include "application.h"

#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) \
                                                         : (x))

// graphics_create() helper functions
static int graphics_create_window(graphics_t graphics);
static int graphics_create_instance(graphics_t graphics);
static int graphics_create_surface(graphics_t graphics);
static int graphics_create_physical_device(graphics_t graphics);
static int graphics_create_logical_device(graphics_t graphics);
static int graphics_create_swapchain(graphics_t graphics);
static int graphics_create_image_views(graphics_t graphics);
static int graphics_create_shader_modules(graphics_t graphics);
static int graphics_create_render_pass(graphics_t graphics);
static int graphics_create_command_pool(graphics_t graphics);
static int graphics_create_command_buffers(graphics_t graphics);
static int graphics_create_vertex_buffer(graphics_t graphics);
static int graphics_create_index_buffer(graphics_t graphics);
static int graphics_create_uniform_buffers(graphics_t graphics);
static int graphics_create_pipeline(graphics_t graphics);
static int graphics_create_framebuffers(graphics_t graphics);
static int graphics_create_sync_objects(graphics_t graphics);

// graphics_render() helper functions;
static int graphics_render_acquire_image(graphics_t graphics);
static int graphics_render_reset_commands(graphics_t graphics);
static int graphics_render_record_commands(graphics_t graphics);
static int graphics_render_queue_submit(graphics_t graphics);
static int graphics_render_queue_present(graphics_t graphics);

// misc. utility functions

static int graphics_util_memory_type(
    VkPhysicalDevice physical_device,
    uint32_t type_filter,
    VkMemoryPropertyFlags properties,
    uint32_t *type);

static int graphics_util_load_shader(graphics_t graphics, const char *shader_file, VkShaderModule *shader_module);

static int graphics_util_create_buffer(
    graphics_t graphics,
    const VkBufferCreateInfo *create_info,
    VkMemoryPropertyFlags properties,
    VkBuffer *buffer,
    VkDeviceMemory *buffer_memory);

static int graphics_util_copy_buffer(
    graphics_t graphics,
    VkBuffer source,
    VkBuffer destination,
    VkDeviceSize size);

int graphics_create(graphics_t *graphics, const char *const resource_directory)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(graphics != NULL, "graphics handle cannot be NULL")
    *graphics = calloc(1, sizeof(struct graphics_s));
    CUBE_ASSERT(*graphics != NULL, "failed to allocate graphics")
    (*graphics)->resource_directory = resource_directory;
    CUBE_ASSERT(graphics_create_window(*graphics) == CUBE_SUCCESS, "failed to create window")
    CUBE_ASSERT(graphics_create_instance(*graphics) == CUBE_SUCCESS, "failed to create instance")
    CUBE_ASSERT(graphics_create_surface(*graphics) == CUBE_SUCCESS, "failed to create surface")
    CUBE_ASSERT(graphics_create_physical_device(*graphics) == CUBE_SUCCESS, "failed to create physical device")
    CUBE_ASSERT(graphics_create_logical_device(*graphics) == CUBE_SUCCESS, "failed to create logical device")
    CUBE_ASSERT(graphics_create_swapchain(*graphics) == CUBE_SUCCESS, "failed to create swapchain")
    CUBE_ASSERT(graphics_create_image_views(*graphics) == CUBE_SUCCESS, "failed to create image views")
    CUBE_ASSERT(graphics_create_shader_modules(*graphics) == CUBE_SUCCESS, "failed to create shader modules")
    CUBE_ASSERT(graphics_create_command_pool(*graphics) == CUBE_SUCCESS, "failed to create command pool")
    CUBE_ASSERT(graphics_create_command_buffers(*graphics) == CUBE_SUCCESS, "failed to create command buffer")
    CUBE_ASSERT(graphics_create_vertex_buffer(*graphics) == CUBE_SUCCESS, "failed to create vertex buffer")
    CUBE_ASSERT(graphics_create_index_buffer(*graphics) == CUBE_SUCCESS, "failed to create index buffer")
    CUBE_ASSERT(graphics_create_uniform_buffers(*graphics) == CUBE_SUCCESS, "failed to create uniform buffers")
    CUBE_ASSERT(graphics_create_render_pass(*graphics) == CUBE_SUCCESS, "failed to create render pass")
    CUBE_ASSERT(graphics_create_pipeline(*graphics) == CUBE_SUCCESS, "failed to create graphics pipeline")
    CUBE_ASSERT(graphics_create_framebuffers(*graphics) == CUBE_SUCCESS, "failed to create framebuffers")
    CUBE_ASSERT(graphics_create_sync_objects(*graphics) == CUBE_SUCCESS, "failed to create sync objects")
    CUBE_END_FUNCTION
}

int graphics_render(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(graphics_render_acquire_image(graphics) == CUBE_SUCCESS, "failed to acquire image")
    CUBE_ASSERT(graphics_render_reset_commands(graphics) == CUBE_SUCCESS, "failed to reset commands")
    CUBE_ASSERT(graphics_render_record_commands(graphics) == CUBE_SUCCESS, "failed to record commands")
    CUBE_ASSERT(graphics_render_queue_submit(graphics) == CUBE_SUCCESS, "failed to submit image")
    CUBE_ASSERT(graphics_render_queue_present(graphics) == CUBE_SUCCESS, "failed to present image")
    CUBE_END_FUNCTION
}

void graphics_destroy(graphics_t graphics)
{
    uint32_t frame_index;
    if (graphics != NULL)
    {
        if (graphics->vk_fence != NULL)
        {
            vkDestroyFence(graphics->vk_device, graphics->vk_fence, NULL);
        }
        if (graphics->vk_render_semaphore != NULL)
        {
            vkDestroySemaphore(
                graphics->vk_device,
                graphics->vk_render_semaphore,
                NULL);
        }
        if (graphics->vk_image_semaphore != NULL)
        {
            vkDestroySemaphore(
                graphics->vk_device,
                graphics->vk_image_semaphore,
                NULL);
        }
        if (graphics->vk_command_buffers != NULL)
        {
            vkFreeCommandBuffers(
                graphics->vk_device,
                graphics->vk_command_pool,
                graphics->vk_image_count,
                graphics->vk_command_buffers);
        }
        if (graphics->vk_command_pool != NULL)
        {
            vkDestroyCommandPool(
                graphics->vk_device,
                graphics->vk_command_pool,
                NULL);
        }
        if (graphics->vk_framebuffers != NULL)
        {
            for (frame_index = 0; frame_index < graphics->vk_image_count; frame_index++)
            {
                vkDestroyFramebuffer(
                    graphics->vk_device,
                    graphics->vk_framebuffers[frame_index],
                    NULL);
            }
            free(graphics->vk_framebuffers);
        }
        if (graphics->vk_graphics_pipeline != NULL)
        {
            vkDestroyPipeline(
                graphics->vk_device,
                graphics->vk_graphics_pipeline,
                NULL);
        }
        if (graphics->vk_render_pass != NULL)
        {
            vkDestroyRenderPass(
                graphics->vk_device,
                graphics->vk_render_pass,
                NULL);
        }
        if (graphics->vk_image_views != NULL)
        {
            for (frame_index = 0; frame_index < graphics->vk_image_count; frame_index++)
            {
                vkDestroyImageView(
                    graphics->vk_device,
                    graphics->vk_image_views[frame_index],
                    NULL);
            }
            free(graphics->vk_image_views);
        }
        if (graphics->vk_images != NULL)
        {
            free(graphics->vk_images);
        }
        if (graphics->vk_index_buffer_memory != NULL)
        {
            vkFreeMemory(graphics->vk_device, graphics->vk_index_buffer_memory, NULL);
        }
        if (graphics->vk_index_buffer != NULL)
        {
            vkDestroyBuffer(graphics->vk_device, graphics->vk_index_buffer, NULL);
        }
        if (graphics->vk_vertex_buffer_memory != NULL)
        {
            vkFreeMemory(graphics->vk_device, graphics->vk_vertex_buffer_memory, NULL);
        }
        if (graphics->vk_vertex_buffer != NULL)
        {
            vkDestroyBuffer(graphics->vk_device, graphics->vk_vertex_buffer, NULL);
        }
        if (graphics->vk_swapchain != NULL)
        {
            vkDestroySwapchainKHR(
                graphics->vk_device,
                graphics->vk_swapchain,
                NULL);
        }
        if (graphics->vk_device != NULL)
        {
            vkDestroyDevice(graphics->vk_device, NULL);
        }
        if (graphics->vk_surface != NULL)
        {
            vkDestroySurfaceKHR(
                graphics->vk_instance,
                graphics->vk_surface,
                NULL);
        }
        if (graphics->vk_instance != NULL)
        {
            vkDestroyInstance(
                graphics->vk_instance,
                NULL);
        }
        SDL_DestroyWindow(graphics->window);
        free(graphics);
    }
}

int graphics_create_window(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    SDL_DisplayMode display_mode;

    CUBE_ASSERT(SDL_GetCurrentDisplayMode(0, &display_mode) >= 0, SDL_GetError())

    graphics->window = SDL_CreateWindow(
        "Johnny's Cube",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        display_mode.w,
        display_mode.h,
        SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN);

    CUBE_ASSERT(graphics->window != NULL, SDL_GetError())
    CUBE_END_FUNCTION
}

int graphics_create_instance(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    const char *instance_layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    uint32_t instance_layer_count = sizeof(instance_layers) / sizeof(instance_layers[0]);

    const VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Johnny's Cube",
        .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .pEngineName = "Johnny's Engine",
        .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 3),
    };
    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
#ifndef NDEBUG
        .enabledLayerCount = instance_layer_count,
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
            (const VkInstanceCreateInfo *)&instance_create_info,
            NULL,
            &graphics->vk_instance))

    CUBE_END_FUNCTION
}

int graphics_create_surface(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(
        SDL_Vulkan_CreateSurface(
            graphics->window,
            graphics->vk_instance,
            &graphics->vk_surface) == SDL_TRUE,
        SDL_GetError())
    CUBE_END_FUNCTION
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
    CUBE_BEGIN_FUNCTION
    const char *const device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    const float queue_priorities[] = {1.0f};
    const VkPhysicalDeviceFeatures device_features = {.samplerAnisotropy = VK_TRUE};
    VkDeviceQueueCreateInfo queue_create_infos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = graphics->vk_graphics_queue_index,
            .pQueuePriorities = &queue_priorities[0],
            .queueCount = 1,
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = graphics->vk_present_queue_index,
            .pQueuePriorities = &queue_priorities[0],
            .queueCount = 1,
        },
    };
    uint32_t unique_queue_count = (graphics->vk_graphics_queue_index != graphics->vk_present_queue_index) ? 2 : 1;
    const VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = unique_queue_count,
        .pQueueCreateInfos = &queue_create_infos[0],
        .enabledExtensionCount = sizeof(device_extensions) / sizeof(device_extensions[0]),
        .ppEnabledExtensionNames = &device_extensions[0],
        .pEnabledFeatures = &device_features,
    };

    CUBE_ASSERT(
        vkCreateDevice(
            graphics->vk_physical_device,
            &device_create_info,
            NULL,
            &graphics->vk_device) == VK_SUCCESS,
        "failed to create device")

    vkGetDeviceQueue(graphics->vk_device, graphics->vk_graphics_queue_index, 0, &graphics->vk_graphics_queue);
    vkGetDeviceQueue(graphics->vk_device, graphics->vk_present_queue_index, 0, &graphics->vk_present_queue);
    CUBE_END_FUNCTION
}

int graphics_create_swapchain(graphics_t graphics)
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
        fprintf(stderr, "graphics_create_swapchain: vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed(%d)\n", vk_result);
        goto error;
    }

    vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(graphics->vk_physical_device, graphics->vk_surface, &surface_format_count, NULL);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swapchain: first call to vkGetPhysicalDeviceSurfaceFormatsKHR failed(%d)\n", vk_result);
        goto error;
    }

    surface_formats = calloc(surface_format_count, sizeof(VkSurfaceFormatKHR));
    if (surface_formats == NULL)
    {
        fputs("graphics_create_swapchain: failed to allocate surface formats\n", stderr);
        goto error;
    }

    vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(graphics->vk_physical_device, graphics->vk_surface, &surface_format_count, surface_formats);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swapchain: second call to vkGetPhysicalDeviceSurfaceFormatsKHR failed(%d)\n", vk_result);
        goto error;
    }

    uint32_t surface_format_index;
    for (surface_format_index = 0; surface_format_index < surface_format_count; surface_format_index++)
    {
        if ((surface_formats + surface_format_index)->format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            graphics->vk_surface_format = *(surface_formats + surface_format_index);
        }
    }

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
        fprintf(stderr, "graphics_create_swapchain: vkCreateSwapchainKHR failed(%d)\n", vk_result);
        goto error;
    }

    vk_result = vkGetSwapchainImagesKHR(graphics->vk_device, graphics->vk_swapchain, &graphics->vk_image_count, NULL);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swapchain: first call to vkGetSwapchainImagesKHR failed(%d)\n", vk_result);
        goto error;
    }

    graphics->vk_images = calloc(graphics->vk_image_count, sizeof(VkImage));
    if (graphics->vk_images == NULL)
    {
        fputs("graphics_create_swapchain: failed to allocate images\n", stderr);
        goto error;
    }

    vk_result = vkGetSwapchainImagesKHR(graphics->vk_device, graphics->vk_swapchain, &graphics->vk_image_count, graphics->vk_images);
    if (vk_result != VK_SUCCESS)
    {
        fprintf(stderr, "graphics_create_swapchain: second call to vkGetSwapchainImagesKHR failed(%d)\n", vk_result);
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
    CUBE_BEGIN_FUNCTION
    VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
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
    uint32_t image_index;

    graphics->vk_image_views = calloc(graphics->vk_image_count, sizeof(VkImageView));
    CUBE_ASSERT(graphics->vk_image_views != NULL, "failed to allocate image views")

    for (image_index = 0; image_index < graphics->vk_image_count; image_index++)
    {
        image_view_create_info.image = *(graphics->vk_images + image_index);
        VK_CHECK_RESULT(
            vkCreateImageView(
                graphics->vk_device,
                &image_view_create_info,
                NULL,
                graphics->vk_image_views + image_index))
    }
    CUBE_END_FUNCTION
}

int graphics_create_shader_modules(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION

    CUBE_ASSERT(
        graphics_util_load_shader(
            graphics, "vert.spv",
            &graphics->vk_vertex_shader) == CUBE_SUCCESS,
        "failed to load vertex shader")

    CUBE_ASSERT(
        graphics_util_load_shader(
            graphics, "frag.spv",
            &graphics->vk_fragment_shader) == CUBE_SUCCESS,
        "failed to load fragment shader")

    CUBE_END_FUNCTION
}

int graphics_create_render_pass(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION

    const VkAttachmentDescription attachments[] = {
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

    const VkAttachmentReference color_reference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkSubpassDescription subpass_description = {
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

    const VkSubpassDependency subpass_dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    };

    const VkRenderPassCreateInfo render_pass_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachments[0],
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 1,
        .pDependencies = &subpass_dependency,
    };

    VK_CHECK_RESULT(vkCreateRenderPass(
        graphics->vk_device,
        &render_pass_create_info,
        NULL,
        &graphics->vk_render_pass))

    CUBE_END_FUNCTION
}

int graphics_create_command_pool(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    const VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = graphics->vk_graphics_queue_index,
    };

    VK_CHECK_RESULT(
        vkCreateCommandPool(
            graphics->vk_device,
            &command_pool_create_info,
            NULL,
            &graphics->vk_command_pool))
    CUBE_END_FUNCTION
}

int graphics_create_command_buffers(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = graphics->vk_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = graphics->vk_image_count,
    };

    graphics->vk_command_buffers = calloc(graphics->vk_image_count, sizeof(VkCommandBuffer));
    CUBE_ASSERT(graphics->vk_command_buffers != NULL, "failed to allocate command buffers")

    VK_CHECK_RESULT(vkAllocateCommandBuffers(
        graphics->vk_device,
        &command_buffer_allocate_info,
        graphics->vk_command_buffers))

    CUBE_END_FUNCTION
}

int graphics_create_vertex_buffer(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    const vertex_t vertices[] = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
    };
    const VkBufferCreateInfo staging_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(vertices),
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    const VkBufferCreateInfo vertex_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(vertices),
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    void *data;

    CUBE_ASSERT(
        graphics_util_create_buffer(
            graphics,
            &staging_buffer_create_info,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_buffer,
            &staging_buffer_memory) == CUBE_SUCCESS,
        "failed to create staging buffer")

    CUBE_ASSERT(
        graphics_util_create_buffer(
            graphics,
            &vertex_buffer_create_info,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &graphics->vk_vertex_buffer,
            &graphics->vk_vertex_buffer_memory) == CUBE_SUCCESS,
        "failed to create vertex buffer")

    VK_CHECK_RESULT(
        vkMapMemory(
            graphics->vk_device,
            staging_buffer_memory,
            0,
            staging_buffer_create_info.size,
            0,
            &data))

    SDL_memcpy(data, &vertices[0], staging_buffer_create_info.size);

    vkUnmapMemory(graphics->vk_device, staging_buffer_memory);

    CUBE_ASSERT(
        graphics_util_copy_buffer(
            graphics,
            staging_buffer,
            graphics->vk_vertex_buffer,
            staging_buffer_create_info.size) == CUBE_SUCCESS,
        "failed to copy staging buffer")

    vkFreeMemory(graphics->vk_device, staging_buffer_memory, NULL);
    vkDestroyBuffer(graphics->vk_device, staging_buffer, NULL);
    CUBE_END_FUNCTION
}

int graphics_create_index_buffer(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    const uint16_t indices[] = {
        0, 1, 2, 2, 3, 0};
    const VkBufferCreateInfo staging_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(indices),
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    const VkBufferCreateInfo vertex_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(indices),
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    void *data;

    CUBE_ASSERT(
        graphics_util_create_buffer(
            graphics,
            &staging_buffer_create_info,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_buffer,
            &staging_buffer_memory) == CUBE_SUCCESS,
        "failed to create staging buffer")

    CUBE_ASSERT(
        graphics_util_create_buffer(
            graphics,
            &vertex_buffer_create_info,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &graphics->vk_index_buffer,
            &graphics->vk_index_buffer_memory) == CUBE_SUCCESS,
        "failed to create vertex buffer")

    VK_CHECK_RESULT(
        vkMapMemory(
            graphics->vk_device,
            staging_buffer_memory,
            0,
            staging_buffer_create_info.size,
            0,
            &data))

    SDL_memcpy(data, &indices[0], staging_buffer_create_info.size);

    vkUnmapMemory(graphics->vk_device, staging_buffer_memory);

    CUBE_ASSERT(
        graphics_util_copy_buffer(
            graphics,
            staging_buffer,
            graphics->vk_index_buffer,
            staging_buffer_create_info.size) == CUBE_SUCCESS,
        "failed to copy staging buffer")

    vkFreeMemory(graphics->vk_device, staging_buffer_memory, NULL);
    vkDestroyBuffer(graphics->vk_device, staging_buffer, NULL);
    CUBE_END_FUNCTION
}

int graphics_create_uniform_buffers(graphics_t graphics)
{
    return 0;
}

int graphics_create_pipeline(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    VkVertexInputBindingDescription vertex_input_binding_descritpion = {
        .binding = 0,
        .stride = sizeof(vertex_t),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    VkVertexInputAttributeDescription vertex_input_attribute_descritpions[] = {
        {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(vertex_t, position),
        },
        {
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(vertex_t, color),
        },
    };
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertex_input_binding_descritpion,
        .vertexAttributeDescriptionCount = 2,
        .pVertexAttributeDescriptions = &vertex_input_attribute_descritpions[0],
    };
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
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
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
    };
    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = &shader_stages[0],
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly_info,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterization_state_info,
        .pMultisampleState = &multisample_state_info,
        .pColorBlendState = &color_blend_state_info,
        .pDynamicState = &dynamic_state_info,
        .renderPass = graphics->vk_render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
    };

    VK_CHECK_RESULT(
        vkCreatePipelineLayout(
            graphics->vk_device,
            &pipeline_layout_info,
            NULL,
            &pipeline_create_info.layout))

    VK_CHECK_RESULT(
        vkCreateGraphicsPipelines(
            graphics->vk_device,
            VK_NULL_HANDLE,
            1,
            &pipeline_create_info,
            NULL,
            &graphics->vk_graphics_pipeline))

    vkDestroyShaderModule(graphics->vk_device, graphics->vk_fragment_shader, NULL);
    vkDestroyShaderModule(graphics->vk_device, graphics->vk_vertex_shader, NULL);
    vkDestroyPipelineLayout(graphics->vk_device, pipeline_create_info.layout, NULL);
    CUBE_END_FUNCTION
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

int graphics_create_sync_objects(graphics_t graphics)
{
    int status;
    VkResult vk_result;
    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
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

    vk_result = vkCreateFence(
        graphics->vk_device,
        &fence_create_info,
        NULL,
        &graphics->vk_fence);
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

int graphics_render_acquire_image(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION

    VK_CHECK_RESULT(
        vkWaitForFences(
            graphics->vk_device,
            1,
            &graphics->vk_fence,
            VK_TRUE, UINT64_MAX))

    VK_CHECK_RESULT(
        vkResetFences(
            graphics->vk_device,
            1,
            &graphics->vk_fence))

    VK_CHECK_RESULT(
        vkAcquireNextImageKHR(
            graphics->vk_device,
            graphics->vk_swapchain,
            UINT64_MAX,
            graphics->vk_image_semaphore,
            VK_NULL_HANDLE,
            &graphics->vk_current_index))

    CUBE_END_FUNCTION
}

int graphics_render_reset_commands(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    VK_CHECK_RESULT(
        vkResetCommandBuffer(
            *(graphics->vk_command_buffers + graphics->vk_current_index), 0))
    CUBE_END_FUNCTION
}

int graphics_render_record_commands(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION
    const VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    const VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };
    const VkRenderPassBeginInfo render_pass_begin_info = {
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
    const VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)graphics->vk_swapchain_size.width,
        .height = (float)graphics->vk_swapchain_size.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    const VkRect2D scissor = {
        .offset = {0, 0},
        .extent = graphics->vk_swapchain_size,
    };
    const VkDeviceSize vertex_buffer_offsets[] = {0};
    VkCommandBuffer command_buffer;

    command_buffer = *(graphics->vk_command_buffers + graphics->vk_current_index);
    VK_CHECK_RESULT(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info))
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
    vkCmdBindVertexBuffers(
        command_buffer,
        0,
        1,
        &graphics->vk_vertex_buffer,
        &vertex_buffer_offsets[0]);
    vkCmdBindIndexBuffer(
        command_buffer,
        graphics->vk_index_buffer,
        0,
        VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, 6, 1, 0, 0, 0);
    vkCmdEndRenderPass(command_buffer);
    VK_CHECK_RESULT(vkEndCommandBuffer(command_buffer))

    CUBE_END_FUNCTION
}

int graphics_render_queue_submit(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION

    const VkPipelineStageFlags wait_dest_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    const VkSubmitInfo queue_submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &graphics->vk_image_semaphore,
        .pWaitDstStageMask = &wait_dest_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = graphics->vk_command_buffers + graphics->vk_current_index,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &graphics->vk_render_semaphore,
    };

    VK_CHECK_RESULT(
        vkQueueSubmit(
            graphics->vk_graphics_queue,
            1,
            &queue_submit_info,
            graphics->vk_fence))

    CUBE_END_FUNCTION
}

int graphics_render_queue_present(graphics_t graphics)
{
    CUBE_BEGIN_FUNCTION

    const VkPipelineStageFlags wait_dest_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    const VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &graphics->vk_render_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &graphics->vk_swapchain,
        .pImageIndices = &graphics->vk_current_index,
    };

    VK_CHECK_RESULT(vkQueuePresentKHR(graphics->vk_present_queue, &present_info))

    VK_CHECK_RESULT(vkQueueWaitIdle(graphics->vk_present_queue))

    CUBE_END_FUNCTION
}

int graphics_util_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties, uint32_t *type)
{
    CUBE_BEGIN_FUNCTION
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    uint32_t index;
    SDL_bool found_type;

    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

    found_type = SDL_FALSE;
    for (index = 0; index < physical_device_memory_properties.memoryTypeCount; index++)
    {
        if (found_type == SDL_FALSE && ((physical_device_memory_properties.memoryTypes[index].propertyFlags & properties) && (type_filter & (1 << index))))
        {
            *type = index;
            found_type = SDL_TRUE;
        }
    }

    CUBE_ASSERT(found_type == SDL_TRUE, "failed to find memory type")

    CUBE_END_FUNCTION
}

int graphics_util_load_shader(graphics_t graphics, const char *shader_file, VkShaderModule *shader_module)
{
    CUBE_BEGIN_FUNCTION
    VkShaderModuleCreateInfo shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    };

    char *shader_path;

    CUBE_ASSERT(
        SDL_asprintf(
            &shader_path,
            "%s%s%s%s%s",
            graphics->resource_directory,
            PATH_SEPARATOR,
            "shaders",
            PATH_SEPARATOR,
            shader_file) >= 0,
        "failed to allocate shader path")

    shader_module_create_info.pCode = SDL_LoadFile(shader_path, &shader_module_create_info.codeSize);

    SDL_free(shader_path);

    CUBE_ASSERT(shader_module_create_info.pCode != NULL, "failed to load shader code")

    VK_CHECK_RESULT(
        vkCreateShaderModule(
            graphics->vk_device,
            &shader_module_create_info,
            NULL,
            shader_module))

    SDL_free((void *)shader_module_create_info.pCode);

    CUBE_END_FUNCTION
}

int graphics_util_create_buffer(
    graphics_t graphics,
    const VkBufferCreateInfo *create_info,
    VkMemoryPropertyFlags properties,
    VkBuffer *buffer,
    VkDeviceMemory *buffer_memory)
{
    CUBE_BEGIN_FUNCTION
    VkMemoryAllocateInfo buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    };

    VkMemoryRequirements buffer_memory_requirements;

    VK_CHECK_RESULT(
        vkCreateBuffer(
            graphics->vk_device,
            create_info,
            NULL,
            buffer))

    vkGetBufferMemoryRequirements(
        graphics->vk_device,
        *buffer,
        &buffer_memory_requirements);

    buffer_allocate_info.allocationSize = buffer_memory_requirements.size;

    CUBE_ASSERT(
        graphics_util_memory_type(
            graphics->vk_physical_device,
            buffer_memory_requirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &buffer_allocate_info.memoryTypeIndex) == CUBE_SUCCESS,
        "failed to find memory type")

    VK_CHECK_RESULT(
        vkAllocateMemory(
            graphics->vk_device,
            &buffer_allocate_info,
            NULL,
            buffer_memory))

    VK_CHECK_RESULT(
        vkBindBufferMemory(
            graphics->vk_device,
            *buffer,
            *buffer_memory, 0))

    CUBE_END_FUNCTION
}

static int graphics_util_copy_buffer(
    graphics_t graphics,
    VkBuffer source,
    VkBuffer destination,
    VkDeviceSize size)
{
    CUBE_BEGIN_FUNCTION
    const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
        .commandPool = graphics->vk_command_pool,
    };
    const VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    const VkBufferCopy buffer_copy = {
        .size = size,
    };
    VkCommandBuffer command_buffer;
    const VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
    };

    VK_CHECK_RESULT(
        vkAllocateCommandBuffers(
            graphics->vk_device,
            &command_buffer_allocate_info,
            &command_buffer))

    VK_CHECK_RESULT(
        vkBeginCommandBuffer(
            command_buffer,
            &command_buffer_begin_info))
    vkCmdCopyBuffer(command_buffer, source, destination, 1, &buffer_copy);

    VK_CHECK_RESULT(
        vkEndCommandBuffer(command_buffer))

    VK_CHECK_RESULT(
        vkQueueSubmit(
            graphics->vk_graphics_queue,
            1,
            &submit_info, VK_NULL_HANDLE))

    VK_CHECK_RESULT(
        vkQueueWaitIdle(
            graphics->vk_graphics_queue))

    CUBE_END_FUNCTION
}