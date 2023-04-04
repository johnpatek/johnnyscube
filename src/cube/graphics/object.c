#include "cube.h"

int graphics_create_object(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const cube_vertex vertices[] = {
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}};
    const uint32_t indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 3, 4, 4, 3, 7,
        3, 2, 7, 7, 2, 6,
        2, 1, 6, 6, 1, 5,
        1, 0, 5, 5, 0, 4};

    graphics->object = SDL_calloc(1, sizeof(cube_object));
    CUBE_ASSERT(graphics->object != NULL, "failed to allocate object")

    CUBE_ASSERT(
        graphics_util_upload_buffer(
            graphics,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            &vertices[0],
            sizeof(vertices),
            &graphics->object->vertex_buffer,
            &graphics->object->vertex_buffer_allocation) == CUBE_SUCCESS,
        "failed to upload vertex buffer")

    CUBE_ASSERT(
        graphics_util_upload_buffer(
            graphics,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            &indices[0],
            sizeof(indices),
            &graphics->object->index_buffer,
            &graphics->object->index_buffer_allocation) == CUBE_SUCCESS,
        "failed to upload index buffer")

    graphics->object->vertex_count = sizeof(vertices) / sizeof(vertices[0]);
    graphics->object->index_count = sizeof(indices) / sizeof(indices[0]);

    CUBE_END_FUNCTION
}

void graphics_destroy_object(cube_graphics *graphics)
{
    if (graphics->object->vertex_buffer != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(
            graphics->allocator,
            graphics->object->vertex_buffer,
            graphics->object->vertex_buffer_allocation);
    }

    if (graphics->object->index_buffer != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(
            graphics->allocator,
            graphics->object->index_buffer,
            graphics->object->index_buffer_allocation);
    }
}
