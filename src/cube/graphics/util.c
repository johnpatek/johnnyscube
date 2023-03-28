#include "cube.h"

static int graphics_util_copy_buffer(
    cube_graphics *graphics,
    VkBuffer source,
    VkBuffer destination,
    VkDeviceSize size);

static int graphics_util_normalize_vector(
    float *vec,
    size_t size,
    float *result);

static int graphics_util_dot_product(
    float *u,
    float *v,
    size_t size,
    float *result);

static int graphics_util_cross_product(
    float u[3],
    float v[3],
    float *result[3]);

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

int graphics_util_rotate_model(
    float model[4][4],
    float angle,
    float axis[3],
    float *result[4][4])
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(result != NULL, "NULL result address")
    const float s = (float)sin(angle);
    const float c = (float)cos(angle);
    const float vec[3] = {
        (1 - c) * axis[0],
        (1 - c) * axis[1],
        (1 - c) * axis[2],
    };

    const float rotation[3][3] = {
        {
            c + vec[0] * axis[0],
            0 + vec[0] * axis[1] + s * axis[2],
            0 + vec[0] * axis[2] - s * axis[1],
        },
        {
            0 + vec[1] * axis[0] - s * axis[2],
            c + vec[1] * axis[1],
            0 + vec[1] * axis[2] + s * axis[0],
        },
        {
            0 + vec[2] * axis[0] + s * axis[1],
            0 + vec[2] * axis[1] - s * axis[0],
            c + vec[2] * axis[2],
        },
    };

    const float rotated_model[4][4] = {
        {
            model[0][0] * rotation[0][0] + model[1][0] * rotation[0][1] + model[2][0] * rotation[0][2],
            model[0][1] * rotation[0][0] + model[1][1] * rotation[0][1] + model[2][1] * rotation[0][2],
            model[0][2] * rotation[0][0] + model[1][2] * rotation[0][1] + model[2][2] * rotation[0][2],
            model[0][3] * rotation[0][0] + model[1][3] * rotation[0][1] + model[2][3] * rotation[0][2],
        },
        {
            model[0][0] * rotation[1][0] + model[1][0] * rotation[1][1] + model[2][0] * rotation[1][2],
            model[0][1] * rotation[1][0] + model[1][1] * rotation[1][1] + model[2][1] * rotation[1][2],
            model[0][2] * rotation[1][0] + model[1][2] * rotation[1][1] + model[2][2] * rotation[1][2],
            model[0][3] * rotation[1][0] + model[1][3] * rotation[1][1] + model[2][3] * rotation[1][2],

        },
        {
            model[0][0] * rotation[2][0] + model[1][0] * rotation[2][1] + model[2][0] * rotation[2][2],
            model[0][1] * rotation[2][0] + model[1][1] * rotation[2][1] + model[2][1] * rotation[2][2],
            model[0][2] * rotation[2][0] + model[1][2] * rotation[2][1] + model[2][2] * rotation[2][2],
            model[0][3] * rotation[2][0] + model[1][3] * rotation[2][1] + model[2][3] * rotation[2][2],

        },
        {
            model[3][0],
            model[3][0],
            model[3][0],
            model[3][0],
        },
    };

    SDL_memcpy(&(*result[0][0]), &(rotated_model[0][0]), sizeof(model));

    CUBE_END_FUNCTION
}

int graphics_util_look_at(
    float eye[3],
    float center[3],
    float up[3],
    float *result[4][4])
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(result != NULL, "NULL result address")
    float f[3] = {
        center[0] - eye[0],
        center[1] - eye[1],
        center[2] - eye[2],
    };
    float u[3];
    float fu[3];
    float s[3];

    graphics_util_normalize_vector(
        &f[0], 3, &f[0]);

    graphics_util_normalize_vector(
        &up[0], 3, &u[0]);

    graphics_util_cross_product(
        f, u, &fu);

    graphics_util_normalize_vector(
        &fu[0], 3, &s[0]);

    graphics_util_cross_product(
        s, f, &u);

    (*result)[0][0] = s[0];
    (*result)[1][0] = s[1];
    (*result)[2][0] = s[2];
    (*result)[0][1] = u[0];
    (*result)[1][1] = u[1];
    (*result)[2][1] = u[2];
    (*result)[0][2] = -1 * f[0];
    (*result)[1][2] = -1 * f[1];
    (*result)[2][2] = -1 * f[2];
    graphics_util_dot_product(
        &s[0], &eye[0], 3, &(*result)[3][0]);
    (*result)[3][0] *= -1.0f;
    graphics_util_dot_product(
        &u[0], &eye[0], 3, &(*result)[3][1]);
    (*result)[3][1] *= -1.0f;
    graphics_util_dot_product(
        &f[0], &eye[0], 3, &(*result)[3][2]);
    (*result)[3][3] = 1.0f;

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

int graphics_util_normalize_vector(
    float *vec,
    size_t size,
    float *result)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(vec != NULL, "NULL input")
    CUBE_ASSERT(size > 0, "invalid size")
    CUBE_ASSERT(result != NULL, "NULL output")
    float square_sum;
    float length;
    size_t index;

    square_sum = 0.0;

    for (index = 0; index < size; index++)
    {
        square_sum += *(vec + index) * *(vec + index);
    }

    length = (float)sqrt((double)square_sum);

    for (index = 0; index < size; index++)
    {
        *(result + index) = *(vec + index) / length;
    }
    CUBE_END_FUNCTION
}

int graphics_util_dot_product(
    float *u,
    float *v,
    size_t size,
    float *result)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(u != NULL, "NULL u")
    CUBE_ASSERT(v != NULL, "NULL v")
    CUBE_ASSERT(result != NULL, "NULL output")
    size_t index;

    *result = 0.0;

    for (index = 0; index < size; index++)
    {
        *result += *(u + index) * *(v + index);
    }

    CUBE_END_FUNCTION
}

int graphics_util_cross_product(
    float u[3],
    float v[3],
    float *result[3])
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(result != NULL, "NULL output")

    (*result)[0] = u[1] * v[2] - u[2] * v[1];
    (*result)[1] = u[2] * v[0] - u[0] * v[2];
    (*result)[2] = u[0] * v[1] - u[1] * v[0];

    CUBE_END_FUNCTION
}

int graphics_util_perspective(
    float fov,
    float aspect,
    float znear,
    float zfar,
    float *result[4][4])
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(result != NULL, "NULL output")

    const float range = (float)tan(fov / 2.0f) * znear;
    const float left = range * aspect * -1.0f;
    const float right = range * aspect;
    const float bottom = range * -1.0f;
    const float top = range;

    (*result)[0][0] = (2.0f * znear) / (right - left);
    (*result)[0][1] = 0.0f;
    (*result)[0][2] = 0.0f;
    (*result)[0][3] = 0.0f;
    (*result)[1][0] = 0.0f;
    (*result)[1][1] = (2.0f * znear) / (top - bottom);
    (*result)[1][2] = 0.0f;
    (*result)[1][3] = 0.0f;
    (*result)[2][2] = -1.0f * (zfar + znear) / (zfar - znear);
    (*result)[2][3] = -1.0f;
    (*result)[3][0] = 0.0f;
    (*result)[3][1] = 0.0f;
    (*result)[3][2] = -1.0f * (2.0f * zfar * znear) / (zfar - znear);
    (*result)[3][3] = 0.0f;

    CUBE_END_FUNCTION
}