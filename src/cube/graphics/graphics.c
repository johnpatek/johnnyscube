#include "cube.h"

int graphics_create(
    cube_graphics **graphics,
    const char *resource_directory)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(graphics != NULL, "NULL graphics handle")

    *graphics = calloc(1, sizeof(cube_graphics));
    CUBE_ASSERT(*graphics != NULL, "failed to allocate graphics")

    CUBE_ASSERT(graphics_create_display(*graphics) == CUBE_SUCCESS, "failed to create display")

    CUBE_ASSERT(graphics_create_device(*graphics) == CUBE_SUCCESS, "failed to create device")

    CUBE_END_FUNCTION
}

int graphics_render(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    cube_frame *frame;

    CUBE_ASSERT(
        graphics_render_acquire_frame(
            graphics, &frame) == CUBE_SUCCESS,
        "failed to acquire frame")

    CUBE_ASSERT(
        graphics_render_draw_frame(
            graphics,
            frame) == CUBE_SUCCESS,
        "failed to draw frame")

    CUBE_ASSERT(
        graphics_render_submit_frame(
            graphics,
            frame) == CUBE_SUCCESS,
        "failed to submit frame")

    CUBE_END_FUNCTION
}

void graphics_destroy(cube_graphics *graphics)
{
    if (graphics != NULL)
    {
        graphics_destroy_device(graphics);
        graphics_destroy_display(graphics);
        free(graphics);
    }
}
