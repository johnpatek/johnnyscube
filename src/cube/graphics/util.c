#include "cube.h"

static int graphics_util_copy_buffer(
    cube_graphics *graphics,
    VkBuffer source,
    VkBuffer destination,
    VkDeviceSize size);

int graphics_util_load_shader(
    cube_graphics *graphics,
    const char *shader_file,
    VkShaderModule *shader_module)
{
    CUBE_BEGIN_FUNCTION
    VkShaderModuleCreateInfo shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    };
    char *shader_path;
    CUBE_ASSERT(
        SDL_asprintf(
            &shader_path,
            "%s%s%s",
            graphics->shader_directory,
            PATH_SEPARATOR,
            shader_file) >= 0,
        "failed to allocate shader path")
    CUBE_PUSH((void *)shader_path);

    shader_module_create_info.pCode = SDL_LoadFile(shader_path, &shader_module_create_info.codeSize);
    CUBE_ASSERT(shader_module_create_info.pCode != NULL, "failed to load shader code")
    CUBE_PUSH((void *)shader_module_create_info.pCode);

    VK_CHECK_RESULT(
        vkCreateShaderModule(
            graphics->logical_device,
            &shader_module_create_info,
            NULL,
            shader_module))
    CUBE_END_FUNCTION
}

int graphics_util_upload_buffer(
    cube_graphics *graphics,
    VkBufferUsageFlags usage,
    const void *data,
    size_t size,
    VkBuffer *buffer,
    VmaAllocation *buffer_allocation)
{
    CUBE_BEGIN_FUNCTION
    const VkBufferCreateInfo staging_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    const VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    const VmaAllocationCreateInfo host_allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_CPU_ONLY,
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
    };
    const VmaAllocationCreateInfo device_allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
    };
    VkBuffer staging_buffer;
    VmaAllocation staging_buffer_allocation;
    VmaAllocationInfo staging_buffer_allocation_info;

    VK_CHECK_RESULT(
        vmaCreateBuffer(
            graphics->allocator,
            &staging_buffer_create_info,
            &host_allocation_create_info,
            &staging_buffer,
            &staging_buffer_allocation,
            &staging_buffer_allocation_info))

    VK_CHECK_RESULT(
        vmaCreateBuffer(
            graphics->allocator,
            &buffer_create_info,
            &device_allocation_create_info,
            buffer,
            buffer_allocation,
            NULL))

    SDL_memcpy(staging_buffer_allocation_info.pMappedData, data, size);

    CUBE_ASSERT(
        graphics_util_copy_buffer(
            graphics,
            staging_buffer,
            *buffer,
            size) == CUBE_SUCCESS,
        "failed to copy buffer")

    vmaDestroyBuffer(graphics->allocator, staging_buffer, staging_buffer_allocation);

    CUBE_END_FUNCTION
}

int graphics_util_copy_buffer(
    cube_graphics *graphics,
    VkBuffer source,
    VkBuffer destination,
    VkDeviceSize size)
{
    CUBE_BEGIN_FUNCTION
    const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
        .commandPool = graphics->command_pool,
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
            graphics->logical_device,
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
            graphics->graphics_queue,
            1,
            &submit_info, VK_NULL_HANDLE))

    VK_CHECK_RESULT(
        vkQueueWaitIdle(
            graphics->graphics_queue))

    CUBE_END_FUNCTION
}
