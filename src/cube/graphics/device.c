#include "cube.h"

static int graphics_create_physical_device(cube_graphics *graphics);
static int graphics_create_queue_families(cube_graphics *graphics);
static int graphics_create_logical_device(cube_graphics *graphics);
static int graphics_create_allocator(cube_graphics *graphics);
static int graphics_create_command_pool(cube_graphics *graphics);

int graphics_create_device(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(graphics_create_physical_device(graphics) == CUBE_SUCCESS, "failed to create physical device")
    CUBE_ASSERT(graphics_create_queue_families(graphics) == CUBE_SUCCESS, "failed to create queue families")
    CUBE_ASSERT(graphics_create_logical_device(graphics) == CUBE_SUCCESS, "failed to create logical device")
    CUBE_ASSERT(graphics_create_allocator(graphics) == CUBE_SUCCESS, "failed to create allocator")
    CUBE_ASSERT(graphics_create_command_pool(graphics) == CUBE_SUCCESS, "failed to create command pool")
    CUBE_END_FUNCTION
}

void graphics_destroy_device(cube_graphics *graphics)
{
    vmaDestroyAllocator(graphics->allocator);
    vkDestroyCommandPool(
        graphics->logical_device,
        graphics->command_pool,
        NULL);
    vkDestroyDevice(graphics->logical_device, NULL);
}

int graphics_create_physical_device(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    uint32_t count = 1;
    vkEnumeratePhysicalDevices(
        graphics->instance,
        &count,
        &graphics->physical_device);
    CUBE_ASSERT(1 > 0, "failed to get device")
    CUBE_END_FUNCTION
}

int graphics_create_queue_families(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    uint32_t queue_family_property_index;
    uint32_t queue_family_property_count;
    VkQueueFamilyProperties *queue_family_properties;
    VkBool32 found_graphics_queue_family;
    VkBool32 found_present_queue_family;

    vkGetPhysicalDeviceQueueFamilyProperties(
        graphics->physical_device,
        &queue_family_property_count,
        NULL);

    queue_family_properties = CUBE_CALLOC(queue_family_property_count, sizeof(VkQueueFamilyProperties));
    CUBE_ASSERT(queue_family_properties != NULL, "failed to allocate queue family properties");

    vkGetPhysicalDeviceQueueFamilyProperties(
        graphics->physical_device,
        &queue_family_property_count,
        queue_family_properties);

    found_graphics_queue_family = VK_FALSE;
    found_present_queue_family = VK_FALSE;
    for (queue_family_property_index = 0; queue_family_property_index < queue_family_property_count; queue_family_property_index++)
    {
        if ((found_graphics_queue_family == VK_FALSE) && ((queue_family_properties + queue_family_property_index)->queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            found_graphics_queue_family = VK_TRUE;
            graphics->graphics_queue_family_index = queue_family_property_index;
        }
        if (found_present_queue_family == VK_FALSE)
        {
            VK_CHECK_RESULT(
                vkGetPhysicalDeviceSurfaceSupportKHR(
                    graphics->physical_device,
                    queue_family_property_index,
                    graphics->surface,
                    &found_present_queue_family))
        }
    }
    CUBE_ASSERT(found_graphics_queue_family == VK_TRUE, "failed to find graphics queue family");
    CUBE_ASSERT(found_present_queue_family == VK_TRUE, "failed to find present queue family");
    CUBE_END_FUNCTION
}

int graphics_create_logical_device(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const char *const device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    const float queue_priorities[] = {1.0};
    const uint32_t unique_queue_count = (graphics->graphics_queue_family_index != graphics->present_queue_family_index) ? 2 : 1;
    const VkPhysicalDeviceFeatures device_features = {.samplerAnisotropy = VK_TRUE};
    VkDeviceQueueCreateInfo queue_create_infos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = graphics->graphics_queue_family_index,
            .pQueuePriorities = &queue_priorities[0],
            .queueCount = 1,
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = graphics->present_queue_family_index,
            .pQueuePriorities = &queue_priorities[0],
            .queueCount = 1,
        },
    };
    const VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = unique_queue_count,
        .pQueueCreateInfos = &queue_create_infos[0],
        .enabledExtensionCount = sizeof(device_extensions) / sizeof(device_extensions[0]),
        .ppEnabledExtensionNames = &device_extensions[0],
        .pEnabledFeatures = &device_features,
    };
    VK_CHECK_RESULT(
        vkCreateDevice(
            graphics->physical_device,
            &device_create_info,
            NULL,
            &graphics->logical_device))
    vkGetDeviceQueue(
        graphics->logical_device,
        graphics->graphics_queue_family_index,
        0,
        &graphics->graphics_queue);
    vkGetDeviceQueue(
        graphics->logical_device,
        graphics->present_queue_family_index,
        0,
        &graphics->present_queue);
    CUBE_END_FUNCTION
}

int graphics_create_allocator(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const VmaAllocatorCreateInfo allocator_create_info = {
        .instance = graphics->instance,
        .physicalDevice = graphics->physical_device,
        .device = graphics->logical_device,
        .vulkanApiVersion = VK_MAKE_API_VERSION(0, 1, 0, 3),
    };
    VK_CHECK_RESULT(
        vmaCreateAllocator(
            &allocator_create_info,
            &graphics->allocator));
    CUBE_END_FUNCTION
}

int graphics_create_command_pool(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = graphics->graphics_queue_family_index,
    };
    VK_CHECK_RESULT(
        vkCreateCommandPool(
            graphics->logical_device,
            &command_pool_create_info,
            NULL,
            &graphics->command_pool))
    CUBE_END_FUNCTION
}